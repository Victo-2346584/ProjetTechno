#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClayPigeon.generated.h"

UCLASS()
class PROJETTECHNOEMERGENT_API AClayPigeon : public AActor
{
    GENERATED_BODY()

public:
    // Constructeur
    AClayPigeon();

protected:
    // Appelé au début du jeu
    virtual void BeginPlay() override;

public:
    // Appelé à chaque frame
    virtual void Tick(float DeltaTime) override;

    // Fonction appelée quand le pigeon est touché
    UFUNCTION(BlueprintCallable, Category = "Clay Pigeon")
    void OnHit();

    // Lance le pigeon avec une vitesse et direction initiales
    UFUNCTION(BlueprintCallable, Category = "Clay Pigeon")
    void Launch(FVector LaunchVelocity, FVector LaunchDirection);

protected:
    // === COMPOSANTS ===

    // Mesh du pigeon (le modèle 3D)
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* PigeonMesh;

    // Composant de physique pour la trajectoire réaliste
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UProjectileMovementComponent* ProjectileMovement;

    // === PROPRIÉTÉS DU PIGEON ===

    // Vitesse de lancement (en cm/s, 3000 = ~108 km/h)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clay Pigeon Properties")
    float LaunchSpeed;

    // Force de gravité (1.0 = gravité normale, 0.5 = demi-gravité)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clay Pigeon Properties")
    float GravityScale;

    // Vitesse de rotation du pigeon dans les airs
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clay Pigeon Properties")
    FRotator RotationSpeed;

    // Durée de vie avant destruction automatique (en secondes)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Clay Pigeon Properties")
    float LifeSpan;

    // === EFFETS VISUELS ===

    // Particules lors de la destruction
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class UParticleSystem* DestructionParticles;

    // Son de destruction
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
    class USoundBase* DestructionSound;

    // Points gagnés quand on touche ce pigeon
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
    int32 ScoreValue;

private:
    // Le pigeon a-t-il déjà été touché?
    bool bHasBeenHit;

    // Temps écoulé depuis le lancement
    float TimeAlive;

    // Fonction helper pour appliquer la rotation
    void ApplyRotation(float DeltaTime);

    // Fonction pour nettoyer et détruire le pigeon
    void DestroyPigeon();
};