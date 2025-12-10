// Author: Jake Rieger
// Created: 12/10/25.
//

#include "Rigidbody2D.hpp"

namespace Nth {
    Rigidbody2D::Rigidbody2D() {
        UpdateMass(mass);
    }

    void Rigidbody2D::UpdateMass(f32 newMass) {
        mass = newMass;
        if (type == BodyType::Static) {
            inverseMass    = 0.0f;
            inverseInertia = 0.0f;
        } else {
            inverseMass = (mass > 0.0f) ? 1.0f / mass : 0.0f;
            // Simple inertia approximation for a point mass
            // This should be recalculated based on collider shape
            inertia        = mass;
            inverseInertia = (inertia > 0.0f) ? 1.0f / inertia : 0.0f;
        }
    }

    void Rigidbody2D::ApplyForce(const Vec2& f) {
        if (type != BodyType::Dynamic) return;
        force += f;
    }

    void Rigidbody2D::ApplyImpulse(const Vec2& impulse) {
        if (type != BodyType::Dynamic) return;
        velocity += impulse * inverseMass;
    }

    void Rigidbody2D::ApplyImpulseAtPoint(const Vec2& impulse, const Vec2& contactPoint, const Vec2& centerOfMass) {
        if (type != BodyType::Dynamic) return;

        // Linear impulse
        velocity += impulse * inverseMass;

        // Angular impulse
        if (!lockRotation) {
            const Vec2 r     = contactPoint - centerOfMass;
            const auto cross = r.x * impulse.y - r.y * impulse.x;
            angularVelocity += cross * inverseInertia;
        }
    }

    void Rigidbody2D::ApplyTorque(f32 t) {
        if (type != BodyType::Dynamic || lockRotation) return;
        torque += t;
    }

    void Rigidbody2D::ClearForces() {
        force  = Vec2(0.0f);
        torque = 0.0f;
    }
}  // namespace Nth