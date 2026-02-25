# FQuat

`FQuat` represents a rotation as a **quaternion** (X, Y, Z, W). Quaternions avoid gimbal lock and enable smooth interpolation (slerp), making them ideal for complex or continuous rotations.

> Header: `#include "Math/Quat.h"` (included via `CoreMinimal.h`)

---

## Construction

```cpp
// Identity (no rotation)
FQuat Identity = FQuat::Identity;

// From axis + angle (radians)
FQuat Q1 = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));   // 90° around Z

// From an FRotator
FRotator Rot(0.0, 45.0, 0.0);
FQuat Q2 = Rot.Quaternion();
// or
FQuat Q3(Rot);

// From Euler angles (degrees) via FRotator round-trip
FQuat Q4 = FRotator(10.0, 20.0, 30.0).Quaternion();

// Find the rotation between two vectors
FQuat Between = FQuat::FindBetweenVectors(FVector::ForwardVector, FVector::RightVector);

// Find the rotation between two unit vectors (faster, assumes normalized)
FQuat BetweenNormals = FQuat::FindBetweenNormals(FVector::ForwardVector, FVector::UpVector);
```

---

## Component Access

```cpp
FQuat Q = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));

// Raw components (rarely manipulated directly)
double X = Q.X;
double Y = Q.Y;
double Z = Q.Z;
double W = Q.W;

// Get axis and angle back out
FVector Axis;
double AngleRad;
Q.ToAxisAndAngle(Axis, AngleRad);
double AngleDeg = FMath::RadiansToDegrees(AngleRad);
```

> **Warning:** Do not modify X/Y/Z/W directly — this can produce an invalid (non-unit) quaternion.

---

## Composition (Combining Rotations)

Order matters: `A * B` applies **B first**, then **A**.

```cpp
FQuat YawQ   = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));
FQuat PitchQ = FQuat(FVector::RightVector, FMath::DegreesToRadians(45.0));

// Apply pitch first, then yaw
FQuat Combined = YawQ * PitchQ;

// Apply an incremental rotation in local space
FQuat LocalDelta = FQuat(FVector::UpVector, FMath::DegreesToRadians(1.0));
FQuat NewLocal = CurrentQuat * LocalDelta;

// Apply an incremental rotation in world space
FQuat NewWorld = LocalDelta * CurrentQuat;
```

---

## Rotating Vectors

```cpp
FQuat Rot90Z = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));

FVector Forward(1.0, 0.0, 0.0);
FVector Rotated = Rot90Z.RotateVector(Forward);       // ≈ (0, 1, 0)
FVector Unrotated = Rot90Z.UnrotateVector(Rotated);   // ≈ (1, 0, 0)
```

---

## Direction Vectors from a Quaternion

```cpp
FQuat Q = FQuat(FVector::UpVector, FMath::DegreesToRadians(45.0));

FVector Fwd   = Q.GetForwardVector();   // rotated X axis
FVector Right = Q.GetRightVector();     // rotated Y axis
FVector Up    = Q.GetUpVector();        // rotated Z axis

FVector RotAxis = Q.GetRotationAxis();
```

---

## Conversion

```cpp
FQuat Q = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));

// To FRotator
FRotator Rot = Q.Rotator();

// To Euler (degrees)
FVector Euler = Q.Euler();   // (Roll, Pitch, Yaw)

// From FRotator
FQuat FromRot = FRotator(10.0, 20.0, 30.0).Quaternion();

// From direction vector (no roll)
FVector Dir(1.0, 1.0, 0.0);
FQuat FromDir = Dir.ToOrientationQuat();
```

---

## Interpolation (Slerp)

Spherical linear interpolation produces smooth, constant-speed rotation.

```cpp
FQuat Start = FQuat::Identity;
FQuat End   = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));

// Interpolate 50% between Start and End
FQuat Mid = FQuat::Slerp(Start, End, 0.5);

// Fast linear interpolation (cheaper, slightly less accurate)
FQuat FastMid = FQuat::FastLerp(Start, End, 0.5);
FastMid.Normalize();  // must normalize after FastLerp
```

---

## Normalization

Quaternions **must** stay normalized to represent valid rotations.

```cpp
FQuat Q = SomeCalculation();

// Check & fix
if (!Q.IsNormalized())
{
    Q.Normalize();
}

// Or get a normalized copy
FQuat Safe = Q.GetNormalized();
```

---

## Inverse

```cpp
FQuat Q = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));

FQuat Inv = Q.Inverse();    // rotation that undoes Q

// Verify: Q * Inv ≈ Identity
FQuat ShouldBeIdentity = Q * Inv;
```

---

## Angular Distance

```cpp
FQuat A = FQuat::Identity;
FQuat B = FQuat(FVector::UpVector, FMath::DegreesToRadians(45.0));

// Returns angle in radians between two quaternions
double AngleRad = A.AngularDistance(B);
double AngleDeg = FMath::RadiansToDegrees(AngleRad);  // 45.0
```

---

## Common Patterns

### Apply incremental rotation each frame (no gimbal lock)
```cpp
FQuat Current = GetActorQuat();
FQuat Delta = FQuat(FVector::UpVector, FMath::DegreesToRadians(RotSpeed * DeltaTime));
SetActorRotation(Current * Delta);
```

### Align an actor to a surface normal
```cpp
FVector SurfaceNormal = HitResult.ImpactNormal;
FQuat AlignQuat = FQuat::FindBetweenVectors(FVector::UpVector, SurfaceNormal);
SetActorRotation(AlignQuat);
```

### Smoothly interpolate rotation
```cpp
FQuat Current = GetActorQuat();
FQuat Target  = TargetActor->GetActorQuat();
FQuat Smooth  = FQuat::Slerp(Current, Target, DeltaTime * InterpSpeed);
SetActorRotation(Smooth);
```

---

## Gotchas

- **Always normalize** after manual math operations (addition, scaling, FastLerp).
- **Composition order**: `A * B` applies B first, then A — the opposite of `FTransform` multiplication.
- **Double cover**: `Q` and `-Q` represent the same rotation. Use `EnforceShortestArcWith()` before interpolating to avoid the "long way around."

---

## See Also

- [FVector](FVector.md) — 3D position / direction
- [FRotator](FRotator.md) — Euler-angle rotation
