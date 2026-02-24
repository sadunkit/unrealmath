# FRotator

`FRotator` stores rotation as **Euler angles** in degrees: **Pitch** (Y-axis), **Yaw** (Z-axis), and **Roll** (X-axis). It is the most common rotation type in gameplay code.

> Header: `#include "Math/Rotator.h"` (included via `CoreMinimal.h`)

---

## Construction

```cpp
// Zero rotation
FRotator Zero = FRotator::ZeroRotator;   // (0, 0, 0)

// From Pitch, Yaw, Roll (all in degrees)
FRotator R1(45.0, 90.0, 0.0);           // Pitch=45, Yaw=90, Roll=0

// From a single value (Pitch = Yaw = Roll)
FRotator R2(0.0);                        // all zero

// From a quaternion
FQuat Quat = FQuat::Identity;
FRotator R3(Quat);

// From a direction vector
FVector Dir(1.0, 1.0, 0.0);
FRotator R4 = Dir.Rotation();            // Yaw ~= 45°
```

---

## Component Access

```cpp
FRotator Rot(15.0, 90.0, 5.0);

double P = Rot.Pitch;   // 15.0   — nose up/down
double Y = Rot.Yaw;     // 90.0   — turn left/right
double R = Rot.Roll;    // 5.0    — tilt sideways
```

> **Unreal's coordinate convention:**
> - **Pitch** rotates around the **Y** axis
> - **Yaw** rotates around the **Z** axis
> - **Roll** rotates around the **X** axis

---

## Arithmetic

```cpp
FRotator A(10.0, 20.0, 0.0);
FRotator B(5.0, -10.0, 0.0);

FRotator Sum  = A + B;     // (15, 10, 0)
FRotator Diff = A - B;     // (5, 30, 0)
FRotator Scaled = A * 2.0; // (20, 40, 0)
```

---

## Normalization & Clamping

```cpp
FRotator R(370.0, -45.0, 0.0);

// Clamp each axis to [0, 360)
FRotator Norm = R.GetNormalized();        // (10, 315, 0)

// Clamp each axis to [-180, 180)
FRotator Denorm = R.GetDenormalized();

// Clamp pitch to avoid gimbal lock issues
FRotator Clamped = R.Clamp();
```

---

## Rotation of Vectors

```cpp
FRotator Rot(0.0, 90.0, 0.0);            // 90° yaw

FVector Forward(1.0, 0.0, 0.0);
FVector Rotated = Rot.RotateVector(Forward);     // (0, 1, 0)
FVector Unrotated = Rot.UnrotateVector(Rotated); // (1, 0, 0)
```

---

## Getting Direction Vectors

```cpp
FRotator Rot(0.0, 45.0, 0.0);

FVector Fwd   = Rot.Vector();                // unit forward direction
FVector Right = FRotationMatrix(Rot).GetUnitAxis(EAxis::Y);
FVector Up    = FRotationMatrix(Rot).GetUnitAxis(EAxis::Z);

// Convenience — same as Rot.Vector()
FVector Forward2 = Rot.RotateVector(FVector::ForwardVector);
```

---

## Conversion

```cpp
FRotator Rot(0.0, 90.0, 0.0);

// To quaternion
FQuat Quat = Rot.Quaternion();

// To direction vector (unit forward)
FVector Dir = Rot.Vector();

// From quaternion
FRotator Back = Quat.Rotator();
```

---

## Interpolation

```cpp
FRotator Start(0.0, 0.0, 0.0);
FRotator End(0.0, 180.0, 0.0);

// Linear interpolation (takes shortest path)
FRotator Mid = FMath::Lerp(Start, End, 0.5);

// Constant-speed rotation toward target
FRotator Step = FMath::RInterpConstantTo(Start, End, DeltaTime, 90.0);

// Smooth rotation (spring-like)
FRotator Smooth = FMath::RInterpTo(Start, End, DeltaTime, 5.0);
```

---

## Common Patterns

### Look at a target
```cpp
FVector MyLocation = GetActorLocation();
FVector TargetLocation = Target->GetActorLocation();
FRotator LookAt = (TargetLocation - MyLocation).Rotation();
SetActorRotation(LookAt);
```

### Rotate an actor smoothly each frame
```cpp
FRotator Current = GetActorRotation();
FRotator Desired(0.0, 90.0, 0.0);
FRotator NewRot = FMath::RInterpTo(Current, Desired, DeltaTime, 2.0);
SetActorRotation(NewRot);
```

### Spin an actor continuously
```cpp
FRotator DeltaRot(0.0, RotationSpeed * DeltaTime, 0.0);
AddActorWorldRotation(DeltaRot);
```

---

## Gotchas

- **Gimbal lock**: Extreme pitch values (~±90°) cause yaw and roll to collapse. Use `FQuat` for complex 3D rotations.
- **Angle wrapping**: Pitch is clamped to [-90, 90] internally; Yaw and Roll wrap at ±180. Use `GetNormalized()` for consistent comparisons.
- **Composition order**: Adding rotators is **not** true rotation composition. For proper composition, convert to `FQuat` and multiply.

---

## See Also

- [FVector](FVector.md) — 3D position / direction
- [FQuat](FQuat.md) — Quaternion rotation (gimbal-lock free)
