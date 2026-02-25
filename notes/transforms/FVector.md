# FVector

`FVector` is a 3D vector (X, Y, Z) used throughout Unreal Engine to represent **positions**, **directions**, **velocities**, and **offsets** in world space.

> Header: `#include "Math/Vector.h"` (included via `CoreMinimal.h`)

---

## Construction

```cpp
// Zero vector
FVector Zero = FVector::ZeroVector;            // (0, 0, 0)

// From components
FVector V1(100.0, 200.0, 300.0);

// Uniform value
FVector V2(50.0);                              // (50, 50, 50)

// Predefined constants
FVector Fwd  = FVector::ForwardVector;         // (1, 0, 0)
FVector Right = FVector::RightVector;          // (0, 1, 0)
FVector Up   = FVector::UpVector;              // (0, 0, 1)
FVector One  = FVector::OneVector;             // (1, 1, 1)
```

---

## Arithmetic

```cpp
FVector A(1.0, 2.0, 3.0);
FVector B(4.0, 5.0, 6.0);

FVector Sum  = A + B;        // (5, 7, 9)
FVector Diff = B - A;        // (3, 3, 3)
FVector Scaled = A * 2.0;    // (2, 4, 6)
FVector Divided = B / 2.0;   // (2, 2.5, 3)

// Component-wise multiply
FVector CompMul = A * B;     // (4, 10, 18)
```

---

## Length & Distance

```cpp
FVector V(3.0, 4.0, 0.0);

double Len   = V.Size();           // 5.0
double LenSq = V.SizeSquared();    // 25.0  (cheaper, skip sqrt)
double Len2D = V.Size2D();         // 5.0   (ignores Z)

FVector PointA(0.0, 0.0, 0.0);
FVector PointB(100.0, 0.0, 0.0);
double Dist = FVector::Dist(PointA, PointB);          // 100.0
double DistSq = FVector::DistSquared(PointA, PointB); // 10000.0
```

---

## Normalization

```cpp
FVector Dir(3.0, 0.0, 4.0);

// Returns a unit vector — does NOT modify the original
FVector UnitDir = Dir.GetSafeNormal();    // (0.6, 0.0, 0.8)

// In-place normalize (returns false if near-zero)
bool bOk = Dir.Normalize();

// Check before use
if (!Dir.IsNearlyZero())
{
    FVector Safe = Dir.GetSafeNormal();
}
```

---

## Dot Product

Returns a scalar indicating how aligned two vectors are.

```cpp
FVector Forward(1.0, 0.0, 0.0);
FVector Target(1.0, 1.0, 0.0);

double Dot = FVector::DotProduct(Forward.GetSafeNormal(), Target.GetSafeNormal());
// Dot > 0  → same general direction
// Dot == 0 → perpendicular
// Dot < 0  → opposite direction
```

---

## Cross Product

Returns a vector perpendicular to both inputs (follows left-hand rule in UE).

```cpp
FVector X(1.0, 0.0, 0.0);
FVector Y(0.0, 1.0, 0.0);

FVector Z = FVector::CrossProduct(X, Y); // (0, 0, 1)
```

---

## Projection & Clamping

```cpp
FVector Velocity(5.0, 3.0, 1.0);
FVector WallNormal(1.0, 0.0, 0.0);

// Project Velocity onto the wall normal
FVector Projected = Velocity.ProjectOnTo(WallNormal);

// Clamp length between min and max
FVector Clamped = Velocity.GetClampedToSize(0.0, 10.0);

// Clamp to a max length
FVector MaxClamped = Velocity.GetClampedToMaxSize(6.0);
```

---

## Rotation & Conversion

```cpp
FVector Direction(1.0, 0.0, 0.0);

// Convert a direction to a rotator (Pitch, Yaw, Roll)
FRotator Rot = Direction.Rotation();

// Convert a direction to a quaternion (no roll)
FQuat Quat = Direction.ToOrientationQuat();

// Rotate a vector by a quaternion
FQuat SomeRotation = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));
FVector Rotated = SomeRotation.RotateVector(Direction);
```

---

## Interpolation

```cpp
FVector Start(0.0, 0.0, 0.0);
FVector End(100.0, 200.0, 0.0);

// Linear interpolation (Alpha 0..1)
FVector Mid = FMath::Lerp(Start, End, 0.5);          // (50, 100, 0)

// Constant-speed interpolation toward target
FVector Step = FMath::VInterpConstantTo(Start, End, DeltaTime, 200.0);

// Smooth interpolation (ease-in / ease-out)
FVector Smooth = FMath::VInterpTo(Start, End, DeltaTime, 5.0);
```

---

## Common Patterns

### Move an actor forward
```cpp
FVector ForwardDir = GetActorForwardVector();
FVector NewLocation = GetActorLocation() + ForwardDir * Speed * DeltaTime;
SetActorLocation(NewLocation);
```

### Check if target is in front
```cpp
FVector ToTarget = (TargetLocation - GetActorLocation()).GetSafeNormal();
double Dot = FVector::DotProduct(GetActorForwardVector(), ToTarget);
bool bInFront = Dot > 0.0;
```

---

## See Also

- [FRotator](FRotator.md) — Euler-angle rotation
- [FQuat](FQuat.md) — Quaternion rotation
