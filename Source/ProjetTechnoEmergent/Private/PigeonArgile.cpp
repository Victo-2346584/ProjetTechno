// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "PigeonArgile.h"

// Sets default values
APigeonArgile::APigeonArgile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    // Active Tick() pour permettre la rotation
    PrimaryActorTick.bCanEverTick = true;

    // === CRÉATION DU MESH (modèle 3D) ===
    PigeonMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PigeonMesh"));
    RootComponent = PigeonMesh;

    // Configure les collisions pour détecter les impacts
    PigeonMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    PigeonMesh->SetCollisionResponseToAllChannels(ECR_Block);
    PigeonMesh->SetSimulatePhysics(false); // On utilise ProjectileMovement au lieu de physique pure

    // === CRÉATION DU COMPOSANT DE MOUVEMENT ===
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = PigeonMesh;
    ProjectileMovement->InitialSpeed = 0.f; // Sera défini dans Launch()
    ProjectileMovement->MaxSpeed = 5000.f;
    ProjectileMovement->bRotationFollowsVelocity = false; // On gère la rotation nous-mêmes
    ProjectileMovement->bShouldBounce = false; // Pas de rebond
    ProjectileMovement->ProjectileGravityScale = 1.0f;

    // === VALEURS PAR DÉFAUT ===
    LaunchSpeed = 3000.f; // ~108 km/h
    GravityScale = 1.0f;
    RotationSpeed = FRotator(180.f, 360.f, 90.f); // Rotation réaliste dans les airs
    LifeSpan = 10.f; // 10 secondes avant disparition
    ScoreValue = 10;

    bHasBeenHit = false;
    TimeAlive = 0.f;
}

// Appelé quand le jeu commence
void APigeonArgile::BeginPlay()
{
    // Configure la durée de vie automatique
    SetLifeSpan(LifeSpan);
}

// Lance le pigeon avec une vitesse et direction
void APigeonArgile::Launch(FVector LaunchVelocity, FVector LaunchDirection)
{
    if (ProjectileMovement)
    {
        // Normalise la direction et applique la vitesse
        FVector Direction = LaunchDirection.GetSafeNormal();
        FVector Velocity = Direction * LaunchSpeed;

        // Si une vélocité personnalisée est fournie, on l'utilise
        if (!LaunchVelocity.IsNearlyZero())
        {
            Velocity = LaunchVelocity;
        }

        // Configure le mouvement
        ProjectileMovement->Velocity = Velocity;
        ProjectileMovement->ProjectileGravityScale = GravityScale;

        // Log pour déboguer
        UE_LOG(LogTemp, Warning, TEXT("Clay Pigeon launched with velocity: %s"), *Velocity.ToString());
    }
}

// Appelé quand le pigeon est touché par un tir
void APigeonArgile::OnHit()
{
    DestroyPigeon();
}
void APigeonArgile::DestroyPigeon()
{
    // Désactive les collisions immédiatement
    PigeonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    // Cache le mesh
    PigeonMesh->SetVisibility(false);

    // Détruit l'acteur après 0.1 seconde (pour laisser les effets se jouer)
    SetLifeSpan(0.1f);
}
