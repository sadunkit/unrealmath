// TransformTests.cpp
// ------------------------------------------------------------------
// SimpleAutomationTests for FVector, FRotator, and FQuat.
//
// HOW TO USE:
//   1. Copy this file into your project's Source/<ModuleName>/Tests/ folder.
//   2. Make sure your .Build.cs includes "Core" in PrivateDependencyModuleNames.
//   3. Compile, then open Window → Test Automation in the Editor.
//   4. Filter for "UnrealMath.Transforms" to find these tests.
//
// Only depends on CoreMinimal.h — no gameplay classes, no world, no actors.
// ------------------------------------------------------------------

#include "CoreMinimal.h"
#include "Misc/AutomationTest.h"

#if WITH_AUTOMATION_TESTS

// ===================================================================
//  Helpers
// ===================================================================

namespace TransformTestHelpers
{
    /** Default tolerance used across all transform tests. */
    static constexpr double Tolerance = 1e-4;

    /** Convenience: compare two FVectors within tolerance. */
    static bool VectorsNearlyEqual(const FVector& A, const FVector& B, double Tol = Tolerance)
    {
        return A.Equals(B, Tol);
    }
}

// ===================================================================
//  FVector Tests
// ===================================================================

// --------------- Construction ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FVectorConstruction,
    "UnrealMath.Transforms.FVector.Construction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVectorConstruction::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    // Zero vector
    TestTrue(TEXT("ZeroVector is (0,0,0)"),
        FVector::ZeroVector.Equals(FVector(0.0, 0.0, 0.0)));

    // Component constructor
    FVector V(1.0, 2.0, 3.0);
    TestEqual(TEXT("X component"), V.X, 1.0);
    TestEqual(TEXT("Y component"), V.Y, 2.0);
    TestEqual(TEXT("Z component"), V.Z, 3.0);

    // Uniform constructor
    FVector Uniform(5.0);
    TestTrue(TEXT("Uniform (5,5,5)"), Uniform.Equals(FVector(5.0, 5.0, 5.0)));

    // Predefined constants
    TestTrue(TEXT("ForwardVector"), FVector::ForwardVector.Equals(FVector(1.0, 0.0, 0.0)));
    TestTrue(TEXT("RightVector"),   FVector::RightVector.Equals(FVector(0.0, 1.0, 0.0)));
    TestTrue(TEXT("UpVector"),      FVector::UpVector.Equals(FVector(0.0, 0.0, 1.0)));

    return true;
}

// --------------- Arithmetic ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FVectorArithmetic,
    "UnrealMath.Transforms.FVector.Arithmetic",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVectorArithmetic::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FVector A(1.0, 2.0, 3.0);
    FVector B(4.0, 5.0, 6.0);

    TestTrue(TEXT("Addition"),       (A + B).Equals(FVector(5.0, 7.0, 9.0)));
    TestTrue(TEXT("Subtraction"),    (B - A).Equals(FVector(3.0, 3.0, 3.0)));
    TestTrue(TEXT("Scalar multiply"), (A * 2.0).Equals(FVector(2.0, 4.0, 6.0)));
    TestTrue(TEXT("Scalar divide"),   (B / 2.0).Equals(FVector(2.0, 2.5, 3.0)));
    TestTrue(TEXT("Component multiply"), (A * B).Equals(FVector(4.0, 10.0, 18.0)));

    return true;
}

// --------------- Length & Distance ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FVectorLengthDistance,
    "UnrealMath.Transforms.FVector.LengthAndDistance",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVectorLengthDistance::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FVector V(3.0, 4.0, 0.0);

    TestNearlyEqual(TEXT("Size"),        V.Size(),        5.0, Tolerance);
    TestNearlyEqual(TEXT("SizeSquared"), V.SizeSquared(), 25.0, Tolerance);

    FVector PointA = FVector::ZeroVector;
    FVector PointB(100.0, 0.0, 0.0);
    TestNearlyEqual(TEXT("Dist"), FVector::Dist(PointA, PointB), 100.0, Tolerance);

    return true;
}

// --------------- Normalization ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FVectorNormalization,
    "UnrealMath.Transforms.FVector.Normalization",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVectorNormalization::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FVector V(3.0, 0.0, 4.0);
    FVector Unit = V.GetSafeNormal();

    TestNearlyEqual(TEXT("Normalized length"), Unit.Size(), 1.0, Tolerance);
    TestTrue(TEXT("Normalized direction"), Unit.Equals(FVector(0.6, 0.0, 0.8), Tolerance));

    // Zero vector returns zero
    FVector Zero = FVector::ZeroVector.GetSafeNormal();
    TestTrue(TEXT("Zero safe normal"), Zero.IsNearlyZero());

    return true;
}

// --------------- Dot Product ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FVectorDotProduct,
    "UnrealMath.Transforms.FVector.DotProduct",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVectorDotProduct::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    // Same direction → 1
    double Same = FVector::DotProduct(FVector::ForwardVector, FVector::ForwardVector);
    TestNearlyEqual(TEXT("Same direction dot"), Same, 1.0, Tolerance);

    // Perpendicular → 0
    double Perp = FVector::DotProduct(FVector::ForwardVector, FVector::RightVector);
    TestNearlyEqual(TEXT("Perpendicular dot"), Perp, 0.0, Tolerance);

    // Opposite → -1
    double Opp = FVector::DotProduct(FVector::ForwardVector, FVector::BackwardVector);
    TestNearlyEqual(TEXT("Opposite dot"), Opp, -1.0, Tolerance);

    return true;
}

// --------------- Cross Product ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FVectorCrossProduct,
    "UnrealMath.Transforms.FVector.CrossProduct",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVectorCrossProduct::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FVector Result = FVector::CrossProduct(FVector::ForwardVector, FVector::RightVector);
    TestTrue(TEXT("X cross Y = Z"), Result.Equals(FVector::UpVector, Tolerance));

    return true;
}

// --------------- Interpolation ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FVectorInterpolation,
    "UnrealMath.Transforms.FVector.Interpolation",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FVectorInterpolation::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FVector Start(0.0, 0.0, 0.0);
    FVector End(100.0, 200.0, 0.0);

    FVector Mid = FMath::Lerp(Start, End, 0.5);
    TestTrue(TEXT("Lerp midpoint"), Mid.Equals(FVector(50.0, 100.0, 0.0), Tolerance));

    FVector AtStart = FMath::Lerp(Start, End, 0.0);
    TestTrue(TEXT("Lerp at 0"), AtStart.Equals(Start, Tolerance));

    FVector AtEnd = FMath::Lerp(Start, End, 1.0);
    TestTrue(TEXT("Lerp at 1"), AtEnd.Equals(End, Tolerance));

    return true;
}

// ===================================================================
//  FRotator Tests
// ===================================================================

// --------------- Construction ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FRotatorConstruction,
    "UnrealMath.Transforms.FRotator.Construction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRotatorConstruction::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FRotator Zero = FRotator::ZeroRotator;
    TestNearlyEqual(TEXT("Zero Pitch"), Zero.Pitch, 0.0, Tolerance);
    TestNearlyEqual(TEXT("Zero Yaw"),   Zero.Yaw,   0.0, Tolerance);
    TestNearlyEqual(TEXT("Zero Roll"),  Zero.Roll,  0.0, Tolerance);

    FRotator R(15.0, 90.0, 5.0);
    TestNearlyEqual(TEXT("Pitch"), R.Pitch, 15.0, Tolerance);
    TestNearlyEqual(TEXT("Yaw"),   R.Yaw,   90.0, Tolerance);
    TestNearlyEqual(TEXT("Roll"),  R.Roll,   5.0,  Tolerance);

    return true;
}

// --------------- Arithmetic ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FRotatorArithmetic,
    "UnrealMath.Transforms.FRotator.Arithmetic",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRotatorArithmetic::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FRotator A(10.0, 20.0, 0.0);
    FRotator B(5.0, -10.0, 0.0);

    FRotator Sum = A + B;
    TestNearlyEqual(TEXT("Sum Pitch"), Sum.Pitch, 15.0,  Tolerance);
    TestNearlyEqual(TEXT("Sum Yaw"),   Sum.Yaw,   10.0,  Tolerance);

    FRotator Diff = A - B;
    TestNearlyEqual(TEXT("Diff Pitch"), Diff.Pitch, 5.0,  Tolerance);
    TestNearlyEqual(TEXT("Diff Yaw"),   Diff.Yaw,  30.0,  Tolerance);

    return true;
}

// --------------- Rotate Vector ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FRotatorRotateVector,
    "UnrealMath.Transforms.FRotator.RotateVector",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRotatorRotateVector::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FRotator Yaw90(0.0, 90.0, 0.0);
    FVector Forward(1.0, 0.0, 0.0);

    FVector Rotated = Yaw90.RotateVector(Forward);
    TestTrue(TEXT("90° Yaw rotates X to Y"), Rotated.Equals(FVector(0.0, 1.0, 0.0), Tolerance));

    FVector Back = Yaw90.UnrotateVector(Rotated);
    TestTrue(TEXT("UnrotateVector reverses"), Back.Equals(Forward, Tolerance));

    return true;
}

// --------------- Conversion to/from FQuat ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FRotatorQuatConversion,
    "UnrealMath.Transforms.FRotator.QuaternionConversion",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRotatorQuatConversion::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FRotator Original(10.0, 45.0, 0.0);
    FQuat Quat = Original.Quaternion();
    FRotator RoundTrip = Quat.Rotator();

    TestNearlyEqual(TEXT("Pitch round-trip"), RoundTrip.Pitch, Original.Pitch, Tolerance);
    TestNearlyEqual(TEXT("Yaw round-trip"),   RoundTrip.Yaw,   Original.Yaw,   Tolerance);
    TestNearlyEqual(TEXT("Roll round-trip"),  RoundTrip.Roll,  Original.Roll,  Tolerance);

    return true;
}

// --------------- Direction Vector ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FRotatorDirectionVector,
    "UnrealMath.Transforms.FRotator.DirectionVector",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FRotatorDirectionVector::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    // Yaw 0 should point along +X
    FRotator NoRot(0.0, 0.0, 0.0);
    FVector Dir = NoRot.Vector();
    TestTrue(TEXT("Zero rotator points forward"), Dir.Equals(FVector::ForwardVector, Tolerance));

    // Yaw 90 should point along +Y
    FRotator Yaw90(0.0, 90.0, 0.0);
    FVector Dir90 = Yaw90.Vector();
    TestTrue(TEXT("Yaw90 points right"), Dir90.Equals(FVector::RightVector, Tolerance));

    return true;
}

// ===================================================================
//  FQuat Tests
// ===================================================================

// --------------- Construction ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FQuatConstruction,
    "UnrealMath.Transforms.FQuat.Construction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FQuatConstruction::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    // Identity
    FQuat Id = FQuat::Identity;
    TestTrue(TEXT("Identity is normalized"), Id.IsNormalized());
    TestNearlyEqual(TEXT("Identity W"), Id.W, 1.0, Tolerance);

    // From axis-angle
    FQuat Q = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));
    TestTrue(TEXT("Axis-angle is normalized"), Q.IsNormalized());

    // From FRotator
    FRotator Rot(0.0, 45.0, 0.0);
    FQuat FromRot = Rot.Quaternion();
    TestTrue(TEXT("From rotator is normalized"), FromRot.IsNormalized());

    return true;
}

// --------------- Rotate Vector ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FQuatRotateVector,
    "UnrealMath.Transforms.FQuat.RotateVector",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FQuatRotateVector::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FQuat Rot90Z = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));
    FVector Forward(1.0, 0.0, 0.0);

    FVector Rotated = Rot90Z.RotateVector(Forward);
    TestTrue(TEXT("90° Z rotates X to Y"), Rotated.Equals(FVector(0.0, 1.0, 0.0), Tolerance));

    FVector Back = Rot90Z.UnrotateVector(Rotated);
    TestTrue(TEXT("UnrotateVector reverses"), Back.Equals(Forward, Tolerance));

    return true;
}

// --------------- Composition ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FQuatComposition,
    "UnrealMath.Transforms.FQuat.Composition",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FQuatComposition::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    // Two 90° rotations around Z should equal 180°
    FQuat Rot90 = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));
    FQuat Rot180 = Rot90 * Rot90;

    FVector Forward(1.0, 0.0, 0.0);
    FVector Result = Rot180.RotateVector(Forward);
    TestTrue(TEXT("Two 90° = 180°"), Result.Equals(FVector(-1.0, 0.0, 0.0), Tolerance));

    return true;
}

// --------------- Inverse ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FQuatInverse,
    "UnrealMath.Transforms.FQuat.Inverse",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FQuatInverse::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FQuat Q = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));
    FQuat Inv = Q.Inverse();
    FQuat Product = Q * Inv;

    // Q * Q^-1 should be identity
    TestTrue(TEXT("Q * Inverse ≈ Identity"), Product.Equals(FQuat::Identity, Tolerance));

    return true;
}

// --------------- Slerp ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FQuatSlerp,
    "UnrealMath.Transforms.FQuat.Slerp",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FQuatSlerp::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FQuat Start = FQuat::Identity;
    FQuat End   = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));

    // Alpha = 0 → Start
    FQuat At0 = FQuat::Slerp(Start, End, 0.0);
    TestTrue(TEXT("Slerp at 0"), At0.Equals(Start, Tolerance));

    // Alpha = 1 → End
    FQuat At1 = FQuat::Slerp(Start, End, 1.0);
    TestTrue(TEXT("Slerp at 1"), At1.Equals(End, Tolerance));

    // Alpha = 0.5 → halfway (45°)
    FQuat Mid = FQuat::Slerp(Start, End, 0.5);
    FVector MidDir = Mid.RotateVector(FVector::ForwardVector);
    // 45° around Z: X → (cos45, sin45, 0)
    double cos45 = FMath::Cos(FMath::DegreesToRadians(45.0));
    double sin45 = FMath::Sin(FMath::DegreesToRadians(45.0));
    TestTrue(TEXT("Slerp midpoint direction"),
        MidDir.Equals(FVector(cos45, sin45, 0.0), Tolerance));

    return true;
}

// --------------- FindBetweenVectors ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FQuatFindBetween,
    "UnrealMath.Transforms.FQuat.FindBetweenVectors",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FQuatFindBetween::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FQuat Between = FQuat::FindBetweenVectors(FVector::ForwardVector, FVector::RightVector);
    FVector Rotated = Between.RotateVector(FVector::ForwardVector);
    TestTrue(TEXT("FindBetween X→Y"), Rotated.Equals(FVector::RightVector, Tolerance));

    return true;
}

// --------------- Angular Distance ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FQuatAngularDistance,
    "UnrealMath.Transforms.FQuat.AngularDistance",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FQuatAngularDistance::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FQuat A = FQuat::Identity;
    FQuat B = FQuat(FVector::UpVector, FMath::DegreesToRadians(45.0));

    double Dist = FMath::RadiansToDegrees(A.AngularDistance(B));
    TestNearlyEqual(TEXT("Angular distance 45°"), Dist, 45.0, Tolerance);

    return true;
}

// ===================================================================
//  Cross-type Conversion Tests
// ===================================================================

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTransformConversions,
    "UnrealMath.Transforms.Conversions.RoundTrips",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransformConversions::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    // FVector → FRotator → FVector
    {
        FVector Dir(1.0, 1.0, 0.0);
        FRotator Rot = Dir.Rotation();
        FVector Back = Rot.Vector();
        TestTrue(TEXT("Vec→Rot→Vec preserves direction"),
            Dir.GetSafeNormal().Equals(Back.GetSafeNormal(), Tolerance));
    }

    // FRotator → FQuat → FRotator
    {
        FRotator Rot(20.0, 60.0, 0.0);
        FQuat Quat = Rot.Quaternion();
        FRotator Back = Quat.Rotator();
        TestNearlyEqual(TEXT("Rot→Quat→Rot Pitch"), Back.Pitch, Rot.Pitch, Tolerance);
        TestNearlyEqual(TEXT("Rot→Quat→Rot Yaw"),   Back.Yaw,   Rot.Yaw,   Tolerance);
        TestNearlyEqual(TEXT("Rot→Quat→Rot Roll"),  Back.Roll,  Rot.Roll,  Tolerance);
    }

    // FVector direction → FQuat → rotate forward vector → same direction
    {
        FVector Dir(1.0, 2.0, 0.5);
        FQuat Quat = Dir.ToOrientationQuat();
        FVector Forward = Quat.GetForwardVector();
        TestTrue(TEXT("Dir→Quat→Forward matches"),
            Dir.GetSafeNormal().Equals(Forward.GetSafeNormal(), Tolerance));
    }

    return true;
}

// ===================================================================
//  FTransform Tests
// ===================================================================

// --------------- Construction ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTransformConstruction,
    "UnrealMath.Transforms.FTransform.Construction",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransformConstruction::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    // Identity transform
    FTransform Identity = FTransform::Identity;
    TestTrue(TEXT("Identity location is zero"), Identity.GetLocation().IsNearlyZero());
    TestTrue(TEXT("Identity rotation is identity"), Identity.GetRotation().Equals(FQuat::Identity, Tolerance));
    TestTrue(TEXT("Identity scale is one"), Identity.GetScale3D().Equals(FVector::OneVector, Tolerance));

    // From location only
    FTransform T1(FVector(100.0, 200.0, 50.0));
    TestTrue(TEXT("Location-only constructor"), T1.GetLocation().Equals(FVector(100.0, 200.0, 50.0), Tolerance));
    TestTrue(TEXT("Default rotation is identity"), T1.GetRotation().Equals(FQuat::Identity, Tolerance));

    // From rotation only
    FQuat Rot90 = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));
    FTransform T2(Rot90);
    TestTrue(TEXT("Rotation-only constructor"), T2.GetRotation().Equals(Rot90, Tolerance));
    TestTrue(TEXT("Default location is zero"), T2.GetLocation().IsNearlyZero());

    // Full constructor (Rotation, Translation, Scale)
    FTransform T3(Rot90, FVector(100.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));
    TestTrue(TEXT("Full constructor rotation"), T3.GetRotation().Equals(Rot90, Tolerance));
    TestTrue(TEXT("Full constructor location"), T3.GetLocation().Equals(FVector(100.0, 0.0, 0.0), Tolerance));
    TestTrue(TEXT("Full constructor scale"), T3.GetScale3D().Equals(FVector(2.0, 2.0, 2.0), Tolerance));

    return true;
}

// --------------- Local to World Conversion ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTransformLocalToWorld,
    "UnrealMath.Transforms.FTransform.LocalToWorld",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransformLocalToWorld::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    // Transform: 90° yaw, position at (100, 0, 0), scale 2
    FQuat Rot90 = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));
    FTransform Transform(Rot90, FVector(100.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));

    // TransformPosition: applies scale, rotation, and translation
    FVector LocalPos(10.0, 0.0, 0.0);  // 10 units forward in local space
    FVector WorldPos = Transform.TransformPosition(LocalPos);
    // After scale (20, 0, 0), after 90° rotation (0, 20, 0), after translation (100, 20, 0)
    TestTrue(TEXT("TransformPosition with scale+rot+trans"),
        WorldPos.Equals(FVector(100.0, 20.0, 0.0), Tolerance));

    // TransformVector: applies scale and rotation, no translation
    FVector LocalDir = FVector::ForwardVector;
    FVector WorldDir = Transform.TransformVector(LocalDir);
    // After scale (2, 0, 0), after 90° rotation (0, 2, 0)
    TestTrue(TEXT("TransformVector with scale+rot"),
        WorldDir.Equals(FVector(0.0, 2.0, 0.0), Tolerance));

    // TransformVectorNoScale: rotation only
    FVector WorldDirNoScale = Transform.TransformVectorNoScale(LocalDir);
    // After 90° rotation: (0, 1, 0)
    TestTrue(TEXT("TransformVectorNoScale rotation only"),
        WorldDirNoScale.Equals(FVector(0.0, 1.0, 0.0), Tolerance));

    return true;
}

// --------------- World to Local Conversion ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTransformWorldToLocal,
    "UnrealMath.Transforms.FTransform.WorldToLocal",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransformWorldToLocal::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FQuat Rot90 = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));
    FTransform Transform(Rot90, FVector(100.0, 0.0, 0.0), FVector(2.0, 2.0, 2.0));

    // InverseTransformPosition
    FVector WorldPos(100.0, 20.0, 0.0);
    FVector LocalPos = Transform.InverseTransformPosition(WorldPos);
    // Remove translation (0, 20, 0), remove rotation (20, 0, 0), remove scale (10, 0, 0)
    TestTrue(TEXT("InverseTransformPosition"),
        LocalPos.Equals(FVector(10.0, 0.0, 0.0), Tolerance));

    // InverseTransformVector
    FVector WorldDir(0.0, 2.0, 0.0);
    FVector LocalDir = Transform.InverseTransformVector(WorldDir);
    // Remove rotation (2, 0, 0), remove scale (1, 0, 0)
    TestTrue(TEXT("InverseTransformVector"),
        LocalDir.Equals(FVector(1.0, 0.0, 0.0), Tolerance));

    // InverseTransformVectorNoScale
    FVector LocalDirNoScale = Transform.InverseTransformVectorNoScale(WorldDir);
    // Remove rotation only: (2, 0, 0)
    TestTrue(TEXT("InverseTransformVectorNoScale"),
        LocalDirNoScale.Equals(FVector(2.0, 0.0, 0.0), Tolerance));

    return true;
}

// --------------- Transform Composition ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTransformComposition,
    "UnrealMath.Transforms.FTransform.Composition",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransformComposition::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    // Parent transform: 90° yaw at (100, 0, 0)
    FQuat ParentRot = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));
    FTransform Parent(ParentRot, FVector(100.0, 0.0, 0.0), FVector::OneVector);

    // Child transform: offset 50 units forward in parent's space
    FTransform Child(FQuat::Identity, FVector(50.0, 0.0, 0.0), FVector::OneVector);

    // Compose: Child * Parent applies Child first, then Parent
    FTransform ChildToWorld = Child * Parent;

    // Child's offset (50, 0, 0) rotated 90° = (0, 50, 0), then translated by (100, 0, 0) = (100, 50, 0)
    TestTrue(TEXT("Composition location"),
        ChildToWorld.GetLocation().Equals(FVector(100.0, 50.0, 0.0), Tolerance));

    // Rotation should be 90° (parent's rotation)
    TestTrue(TEXT("Composition rotation"),
        ChildToWorld.GetRotation().Equals(ParentRot, Tolerance));

    return true;
}

// --------------- GetRelativeTransform ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTransformGetRelative,
    "UnrealMath.Transforms.FTransform.GetRelativeTransform",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransformGetRelative::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FTransform A(FQuat::Identity, FVector(100.0, 0.0, 0.0), FVector::OneVector);
    FTransform B(FQuat::Identity, FVector(150.0, 50.0, 0.0), FVector::OneVector);

    // B relative to A
    FTransform BRelativeToA = B.GetRelativeTransform(A);

    // B is 50 units forward and 50 units right of A
    TestTrue(TEXT("GetRelativeTransform location"),
        BRelativeToA.GetLocation().Equals(FVector(50.0, 50.0, 0.0), Tolerance));

    // Reconstruct B from A and relative transform
    FTransform ReconstructedB = BRelativeToA * A;
    TestTrue(TEXT("Reconstruction matches original"),
        ReconstructedB.GetLocation().Equals(B.GetLocation(), Tolerance));

    return true;
}

// --------------- Inverse ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTransformInverse,
    "UnrealMath.Transforms.FTransform.Inverse",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransformInverse::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FQuat Rot45 = FQuat(FVector::UpVector, FMath::DegreesToRadians(45.0));
    FTransform T(Rot45, FVector(100.0, 50.0, 0.0), FVector(2.0, 2.0, 2.0));

    FTransform InvT = T.Inverse();

    // T * InvT should be identity
    FTransform Identity = T * InvT;
    TestTrue(TEXT("T * Inverse ≈ Identity location"),
        Identity.GetLocation().IsNearlyZero(Tolerance));
    TestTrue(TEXT("T * Inverse ≈ Identity rotation"),
        Identity.GetRotation().Equals(FQuat::Identity, Tolerance));
    TestTrue(TEXT("T * Inverse ≈ Identity scale"),
        Identity.GetScale3D().Equals(FVector::OneVector, Tolerance));

    // Round-trip: transform and inverse should recover original point
    FVector Original(25.0, 75.0, 10.0);
    FVector Transformed = T.TransformPosition(Original);
    FVector RoundTrip = InvT.TransformPosition(Transformed);
    TestTrue(TEXT("Inverse round-trip"),
        RoundTrip.Equals(Original, Tolerance));

    return true;
}

// --------------- Interpolation (Blend) ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTransformBlend,
    "UnrealMath.Transforms.FTransform.Blend",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransformBlend::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FTransform Start(FQuat::Identity, FVector(0.0, 0.0, 0.0), FVector(1.0, 1.0, 1.0));
    FTransform End(FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0)),
                   FVector(100.0, 0.0, 0.0),
                   FVector(2.0, 2.0, 2.0));

    // Alpha = 0 → Start
    FTransform At0;
    At0.Blend(Start, End, 0.0f);
    TestTrue(TEXT("Blend at 0 location"), At0.GetLocation().Equals(Start.GetLocation(), Tolerance));
    TestTrue(TEXT("Blend at 0 rotation"), At0.GetRotation().Equals(Start.GetRotation(), Tolerance));

    // Alpha = 1 → End
    FTransform At1;
    At1.Blend(Start, End, 1.0f);
    TestTrue(TEXT("Blend at 1 location"), At1.GetLocation().Equals(End.GetLocation(), Tolerance));
    TestTrue(TEXT("Blend at 1 rotation"), At1.GetRotation().Equals(End.GetRotation(), Tolerance));

    // Alpha = 0.5 → Midpoint
    FTransform Mid;
    Mid.Blend(Start, End, 0.5f);
    TestTrue(TEXT("Blend at 0.5 location"),
        Mid.GetLocation().Equals(FVector(50.0, 0.0, 0.0), Tolerance));
    TestTrue(TEXT("Blend at 0.5 scale"),
        Mid.GetScale3D().Equals(FVector(1.5, 1.5, 1.5), Tolerance));

    // Rotation should be 45° (halfway between 0° and 90°)
    FVector MidForward = Mid.GetRotation().RotateVector(FVector::ForwardVector);
    double Cos45 = FMath::Cos(FMath::DegreesToRadians(45.0));
    double Sin45 = FMath::Sin(FMath::DegreesToRadians(45.0));
    TestTrue(TEXT("Blend at 0.5 rotation direction"),
        MidForward.Equals(FVector(Cos45, Sin45, 0.0), Tolerance));

    return true;
}

// --------------- GetUnitAxis ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTransformGetUnitAxis,
    "UnrealMath.Transforms.FTransform.GetUnitAxis",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransformGetUnitAxis::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    // 90° yaw rotation
    FQuat Rot90 = FQuat(FVector::UpVector, FMath::DegreesToRadians(90.0));
    FTransform T(Rot90, FVector::ZeroVector, FVector::OneVector);

    FVector Forward = T.GetUnitAxis(EAxis::X);
    FVector Right = T.GetUnitAxis(EAxis::Y);
    FVector Up = T.GetUnitAxis(EAxis::Z);

    // After 90° yaw: Forward (X) → Right (Y), Right (Y) → Backward (-X), Up (Z) → Up (Z)
    TestTrue(TEXT("Forward axis after 90° yaw"),
        Forward.Equals(FVector(0.0, 1.0, 0.0), Tolerance));
    TestTrue(TEXT("Right axis after 90° yaw"),
        Right.Equals(FVector(-1.0, 0.0, 0.0), Tolerance));
    TestTrue(TEXT("Up axis after 90° yaw"),
        Up.Equals(FVector(0.0, 0.0, 1.0), Tolerance));

    return true;
}

// --------------- Round-Trip Conversions ---------------

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
    FTransformRoundTrips,
    "UnrealMath.Transforms.FTransform.RoundTrips",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FTransformRoundTrips::RunTest(const FString& Parameters)
{
    using namespace TransformTestHelpers;

    FQuat Rot = FQuat(FVector::UpVector, FMath::DegreesToRadians(45.0));
    FTransform T(Rot, FVector(100.0, 50.0, 25.0), FVector(2.0, 2.0, 2.0));

    // Position round-trip
    {
        FVector Local(10.0, 20.0, 5.0);
        FVector World = T.TransformPosition(Local);
        FVector BackToLocal = T.InverseTransformPosition(World);
        TestTrue(TEXT("Position round-trip"),
            BackToLocal.Equals(Local, Tolerance));
    }

    // Vector round-trip
    {
        FVector LocalDir = FVector(1.0, 0.5, 0.2).GetSafeNormal();
        FVector WorldDir = T.TransformVector(LocalDir);
        FVector BackToLocal = T.InverseTransformVector(WorldDir);
        TestTrue(TEXT("Vector round-trip"),
            BackToLocal.Equals(LocalDir, Tolerance));
    }

    // Inverse round-trip
    {
        FTransform Inv = T.Inverse();
        FTransform InvInv = Inv.Inverse();
        TestTrue(TEXT("Inverse round-trip location"),
            InvInv.GetLocation().Equals(T.GetLocation(), Tolerance));
        TestTrue(TEXT("Inverse round-trip rotation"),
            InvInv.GetRotation().Equals(T.GetRotation(), Tolerance));
        TestTrue(TEXT("Inverse round-trip scale"),
            InvInv.GetScale3D().Equals(T.GetScale3D(), Tolerance));
    }

    return true;
}

#endif // WITH_AUTOMATION_TESTS
