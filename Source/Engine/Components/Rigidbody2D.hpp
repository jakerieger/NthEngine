// Author: Jake Rieger
// Created: 12/10/25.
//

#pragma once

#include "CommonPCH.hpp"

namespace Nth {
    enum class BodyType {
        Static,    // Infinite mass, doesn't move (e.g., walls, ground)
        Dynamic,   // Affected by forces and gravity
        Kinematic  // Movable but not affected by forces (e.g., moving platforms)
    };

    /// @brief 2D rigid body physics component for dynamic simulation
    ///
    /// Manages physical properties and forces for 2D objects in a physics simulation.
    /// Supports linear and angular motion, mass properties, material properties,
    /// and various force application methods.
    struct Rigidbody2D {
        /// @brief Type of physics body (Dynamic, Kinematic, or Static)
        BodyType type = BodyType::Dynamic;

        /// @brief Linear velocity in units per second
        Vec2 velocity {0.0f};

        /// @brief Linear acceleration in units per second squared
        Vec2 acceleration {0.0f};

        /// @brief Accumulated force to be applied this physics step
        Vec2 force {0.0f};

        /// @brief Angular velocity in radians per second
        f32 angularVelocity {0.0f};

        /// @brief Angular acceleration in radians per second squared
        f32 angularAcceleration {0.0f};

        /// @brief Accumulated torque to be applied this physics step
        f32 torque {0.0f};

        /// @brief Mass of the body in kilograms
        f32 mass {1.0f};

        /// @brief Precomputed inverse mass (1/mass) for optimization
        f32 inverseMass {1.0f};

        /// @brief Rotational inertia (resistance to angular acceleration)
        f32 inertia {1.0f};

        /// @brief Precomputed inverse inertia (1/inertia) for optimization
        f32 inverseInertia {1.0f};

        /// @brief Coefficient of restitution (bounciness), range [0, 1]
        /// 0 = perfectly inelastic (no bounce), 1 = perfectly elastic (full bounce)
        f32 restitution {0.5f};

        /// @brief Coefficient of friction, range [0, 1]
        /// 0 = frictionless, 1 = high friction
        f32 friction {0.3f};

        /// @brief Linear velocity damping factor per second
        /// Higher values cause faster velocity decay
        f32 linearDamping {0.01f};

        /// @brief Angular velocity damping factor per second
        /// Higher values cause faster rotational decay
        f32 angularDamping {0.01f};

        /// @brief Multiplier for gravity effect on this body
        /// 0 = no gravity, 1 = normal gravity, >1 = increased gravity
        f32 gravityScale {1.0f};

        /// @brief If true, prevents rotation of the body
        bool lockRotation {false};

        /// @brief Default constructor
        Rigidbody2D();

        /// @brief Updates the mass and recalculates inverse mass
        /// @param newMass New mass value in kilograms (must be positive)
        void UpdateMass(f32 newMass);

        /// @brief Applies a continuous force to the body's center of mass
        /// @param f Force vector to apply
        void ApplyForce(const Vec2& f);

        /// @brief Applies an instantaneous impulse to the body's center of mass
        /// @param impulse Impulse vector (immediate velocity change)
        void ApplyImpulse(const Vec2& impulse);

        /// @brief Applies an impulse at a specific point, generating both linear and angular effects
        /// @param impulse Impulse vector to apply
        /// @param contactPoint World position where impulse is applied
        /// @param centerOfMass World position of the body's center of mass
        void ApplyImpulseAtPoint(const Vec2& impulse, const Vec2& contactPoint, const Vec2& centerOfMass);

        /// @brief Applies a torque (rotational force) to the body
        /// @param t Torque value in newton-meters
        void ApplyTorque(f32 t);

        /// @brief Clears all accumulated forces and torques
        /// Typically called after physics integration step
        void ClearForces();
    };
}  // namespace Nth
