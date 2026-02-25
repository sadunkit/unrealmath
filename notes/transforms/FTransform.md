# FTransform

## Overview
`FTransform` is Unreal Engine's primary type for representing a full 3D transformation consisting of **Translation** (position), **Rotation** (as a quaternion), and **Scale**. It's the foundation for converting between coordinate spaces—most importantly between **local** and **world** space.

Use `FTransform` when you need to:
- Store and manipulate a complete object transformation (position + rotation + scale)
- Convert coordinates between local and world space
- Compose transformations hierarchically (parent-child relationships)
- Interpolate between full transforms (position, rotation, and scale together)

## Construction

```cpp
// Default constructor — Identity transform (no translation, no rotation, uniform scale of 1)
FTransform Identity = FTransform::Identity;

// From location only (rotation defaults to identity, scale to (1,1,1))
FTransform T1(FVector(100.0, 200.0, 50.0));

// From rotation only (location defaults to zero, scale to (1,1,1))
FTransform T2(FQuat(FVector::UpVector, FMath::DegreesToRadians(45.0)));

// Full constructor: Rotation, Translation, Scale (note the order!)
FTransform T3(
    FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0)), // Rotation
    FVector(100.0, 0.0, 0.0),                                 // Translation
    FVector(1.0, 1.0, 1.0)                                    // Scale
);

// Using setters
FTransform T4;
T4.SetLocation(FVector(10.0, 20.0, 30.0));
T4.SetRotation(FQuat::Identity);
T4.SetScale3D(FVector(2.0, 2.0, 2.0));

// From FRotator
FRotator Rot(0.0, 90.0, 0.0);
FTransform T5(Rot.Quaternion(), FVector::ZeroVector, FVector::OneVector);
```

**Note**: The full constructor parameter order is **(Rotation, Translation, Scale)**, not the alphabetical TRS order you might expect!

## Accessing Components

```cpp
FTransform T = SomeActor->GetActorTransform();

FVector Location = T.GetLocation();        // or T.GetTranslation()
FQuat Rotation = T.GetRotation();
FVector Scale = T.GetScale3D();

// Getting axes
FVector Forward = T.GetUnitAxis(EAxis::X);  // Local forward
FVector Right   = T.GetUnitAxis(EAxis::Y);  // Local right
FVector Up      = T.GetUnitAxis(EAxis::Z);  // Local up
```

## Local ⟺ World Space Conversions

This is where `FTransform` truly shines. Understanding these operations is critical for gameplay programming.

### Converting Local → World

When you have a **local** offset or direction and want to know where it is in **world** space:

```cpp
FTransform ActorTransform = MyActor->GetActorTransform();

// Local position to world position
FVector LocalOffset(100.0, 0.0, 50.0);  // 100cm forward, 50cm up in local space
FVector WorldPosition = ActorTransform.TransformPosition(LocalOffset);

// Local direction to world direction (ignores translation, applies rotation and scale)
FVector LocalDirection = FVector::ForwardVector;
FVector WorldDirection = ActorTransform.TransformVector(LocalDirection);

// Alternate: transform without scale (useful for normals)
FVector WorldDirectionNoScale = ActorTransform.TransformVectorNoScale(LocalDirection);
```

**Key methods**:
- `TransformPosition(FVector)` — Applies **Scale → Rotate → Translate** to a position
- `TransformVector(FVector)` — Applies **Scale → Rotate** (no translation) to a direction
- `TransformVectorNoScale(FVector)` — Applies **Rotate** only (no scale or translation)

### Converting World → Local

When you have a **world** position or direction and want to express it in **local** space:

```cpp
FTransform ActorTransform = MyActor->GetActorTransform();

// World position to local position
FVector WorldPosition = SomeOtherActor->GetActorLocation();
FVector LocalPosition = ActorTransform.InverseTransformPosition(WorldPosition);

// World direction to local direction
FVector WorldDirection = (TargetLocation - MyActor->GetActorLocation()).GetSafeNormal();
FVector LocalDirection = ActorTransform.InverseTransformVector(WorldDirection);

// Without scale
FVector LocalDirectionNoScale = ActorTransform.InverseTransformVectorNoScale(WorldDirection);
```

**Key methods**:
- `InverseTransformPosition(FVector)` — Converts world position to local position
- `InverseTransformVector(FVector)` — Converts world direction to local direction
- `InverseTransformVectorNoScale(FVector)` — Converts world direction to local (ignoring scale)

### Relative Transforms Between Objects

To get the transform of B **relative to** A (express B in A's local space):

```cpp
FTransform WorldA = ActorA->GetActorTransform();
FTransform WorldB = ActorB->GetActorTransform();

// B relative to A (B in A's local space)
FTransform RelativeTransform = WorldB.GetRelativeTransform(WorldA);

// To go back: A * Relative = B in world space
FTransform ReconstructedB = RelativeTransform * WorldA;
```

**Tip**: `GetRelativeTransform(Other)` is equivalent to `this * Other.Inverse()`.

## Transform Composition (Multiplication)

Composing transforms follows the **right-to-left** multiplication order:

```cpp
FTransform LocalToParent = ChildComponent->GetRelativeTransform();
FTransform ParentToWorld = ParentComponent->GetComponentTransform();

// Apply LocalToParent FIRST, then ParentToWorld
FTransform LocalToWorld = LocalToParent * ParentToWorld;
```

**CRITICAL**: `A * B` means "apply A first, then B". This is the **opposite** of quaternion multiplication order.

### Examples

```cpp
FTransform Base = Actor->GetActorTransform();
FTransform DeltaRotation(FQuat(FVector::UpVector, FMath::DegreesToRadians(45.0)));

// Rotate in LOCAL space (delta applied first)
FTransform RotatedLocal = DeltaRotation * Base;

// Rotate in WORLD space (base applied first)
FTransform RotatedWorld = Base * DeltaRotation;
```

### Inverse

```cpp
FTransform T = SomeTransform;
FTransform InvT = T.Inverse();

// T * InvT ≈ Identity
FTransform Identity = T * InvT;
```

**Performance note**: Inverse is relatively cheap but not free. Cache it if you're using it repeatedly in the same frame.

## Interpolation

For smooth movement and animation:

```cpp
FTransform Start = StartLocation;
FTransform End = EndLocation;

// Linear interpolation (0.0 = Start, 1.0 = End)
float Alpha = 0.5f;
FTransform Mid;
Mid.Blend(Start, End, Alpha);  // Modifies Mid in place

// Alternative: static blend
FTransform Blended;
Blended.Blend(Start, End, Alpha);
```

**What happens during `Blend`**:
- **Translation**: Linear interpolation (Lerp)
- **Rotation**: Spherical interpolation (Slerp) for smooth rotation
- **Scale**: Linear interpolation (Lerp) — can be exponential in some cases

For delta-time based smooth following:

```cpp
void Tick(float DeltaTime)
{
    FTransform Current = GetActorTransform();
    FTransform Target = TargetTransform;
    
    float InterpSpeed = 5.0f;  // Adjust for desired smoothness
    float Alpha = FMath::Clamp(DeltaTime * InterpSpeed, 0.0f, 1.0f);
    
    FTransform Interpolated;
    Interpolated.Blend(Current, Target, Alpha);
    
    SetActorTransform(Interpolated);
}
```

## Common Gameplay Patterns

### 1. Spawning Object at Socket

```cpp
USkeletalMeshComponent* Mesh = Character->GetMesh();
FTransform SocketTransform = Mesh->GetSocketTransform(TEXT("hand_r"));

// Spawn with offset in socket's local space
FTransform LocalOffset(FQuat::Identity, FVector(10.0, 0.0, 0.0), FVector::OneVector);
FTransform SpawnTransform = LocalOffset * SocketTransform;

GetWorld()->SpawnActor<AWeapon>(WeaponClass, SpawnTransform);
```

### 2. Converting Aim Direction to Local Space

```cpp
// World space aim direction
FVector WorldAimDir = (TargetLocation - Character->GetActorLocation()).GetSafeNormal();

// Convert to local space for animation blending
FTransform CharTransform = Character->GetActorTransform();
FVector LocalAimDir = CharTransform.InverseTransformVectorNoScale(WorldAimDir);

// Now use LocalAimDir.X, LocalAimDir.Y, LocalAimDir.Z for aim offsets
```

### 3. Projecting Position onto Actor's Plane

```cpp
FTransform ActorTransform = MyActor->GetActorTransform();

// Convert world position to local space
FVector LocalPos = ActorTransform.InverseTransformPosition(WorldPosition);

// Zero out Z to project onto XY plane (actor's "ground")
LocalPos.Z = 0.0;

// Convert back to world
FVector ProjectedWorld = ActorTransform.TransformPosition(LocalPos);
```

### 4. Hierarchical Transform Chains

```cpp
// Character → Weapon → Muzzle
FTransform CharToWorld = Character->GetActorTransform();
FTransform WeaponToChar = Weapon->GetAttachParentSocketTransform();
FTransform MuzzleToWeapon = Weapon->GetMuzzleOffset();

// Muzzle in world space
FTransform MuzzleToWorld = MuzzleToWeapon * WeaponToChar * CharToWorld;

// Spawn projectile here
FVector MuzzleWorldLocation = MuzzleToWorld.GetLocation();
FVector MuzzleWorldForward = MuzzleToWorld.GetUnitAxis(EAxis::X);
```

## Performance Tips

### 1. Use Vectorized Implementation
FTransform internally uses SIMD instructions (SSE, NEON) for optimal performance. The operations are implemented in `TransformVectorized.h` on most platforms.

### 2. Avoid Redundant Inverse Calls
```cpp
// ❌ Bad: Recomputing inverse every iteration
for (FVector& Point : Points)
{
    Point = Transform.InverseTransformPosition(Point);  // Inverse computed each time!
}

// ✅ Good: Cache the inverse
FTransform InvTransform = Transform.Inverse();
for (FVector& Point : Points)
{
    Point = InvTransform.TransformPosition(Point);  // Use cached inverse
}
```

### 3. Use NoScale Variants When Appropriate
If you know scale is uniform or you don't need it, use the `NoScale` methods:
```cpp
// Faster if scale isn't needed
FVector WorldDir = Transform.TransformVectorNoScale(LocalDir);
```

### 4. Prefer Transform Methods Over Manual Math
```cpp
// ❌ Bad: Manual transform
FVector Result = Transform.GetRotation().RotateVector(LocalPos * Transform.GetScale3D()) + Transform.GetLocation();

// ✅ Good: Use built-in method
FVector Result = Transform.TransformPosition(LocalPos);
```

The built-in methods are vectorized and optimized.

## Gotchas

### 1. Non-Uniform Scale + Rotation ≠ Shear
FTransform **cannot represent shear**. When composing transforms with non-uniform scale and rotation, the math doesn't preserve the true geometric transformation:

```cpp
// Parent has non-uniform scale (2, 1, 1) and 45° rotation
FTransform Parent(FQuat(FVector::UpVector, FMath::DegreesToRadians(45.0)), FVector::ZeroVector, FVector(2.0, 1.0, 1.0));

// Child has 45° rotation relative to parent
FTransform Child(FQuat(FVector::UpVector, FMath::DegreesToRadians(45.0)), FVector(100.0, 0.0, 0.0), FVector::OneVector);

// Composition doesn't produce true shear — scale is "pushed" into child's local axes
FTransform Composed = Child * Parent;
```

**Workaround**: Use `FMatrix` if you truly need shear, but note that it's significantly slower.

### 2. Multiplication Order is Opposite of Quaternions
- `FQuat`: `Q1 * Q2` means apply Q2 first, then Q1
- `FTransform`: `T1 * T2` means apply T1 first, then T2

**Why?** FTransform is designed for composing transformations in parent-child hierarchies where you read left-to-right.

### 3. Rotation Representation is Quaternion
Unlike components which often expose `FRotator`, FTransform stores rotation as `FQuat` internally:

```cpp
FTransform T;

// ❌ Wrong: Can't set FRotator directly
// T.SetRotation(FRotator(0, 90, 0));  // Compile error

// ✅ Correct: Convert to quaternion
T.SetRotation(FRotator(0.0, 90.0, 0.0).Quaternion());
```

### 4. Blend() Modifies the Calling Object
```cpp
FTransform Result;
Result.Blend(A, B, 0.5f);  // Result is modified in place
```

This is not a static method — it modifies the object you call it on.

### 5. GetRelativeTransform Argument Order
```cpp
// B.GetRelativeTransform(A) returns B relative to A
// i.e., expresses B in A's local space

FTransform BRelativeToA = B.GetRelativeTransform(A);

// Reconstruction: A * BRelativeToA = B (in world space)
```

Don't confuse the "relative to" direction!

## See Also
- [FVector.md](FVector.md) — For translation/position operations
- [FRotator.md](FRotator.md) — For Euler angle rotation representation
- [FQuat.md](FQuat.md) — For quaternion rotation math
