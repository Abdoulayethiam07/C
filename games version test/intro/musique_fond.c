#include <stdio.h>
#include <raylib.h>

#define SCREENWIDTH 800
#define SCREENHEIGHT 600

int main(void)
{
    // Fenêtre
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Musique de fond");
    SetTargetFPS(60);

    // Audio
    InitAudioDevice();

    // Charger musique
    Music musique = LoadMusicStream("music/musique.mp3");

    // Vérification
    if (!IsMusicValid(musique))
    {
        printf("Erreur chargement musique\n");
        return 1;
    }

    // Variables
    float volume = 0.5f;
    bool musiqueEnPause = false;

    // Jouer musique
    PlayMusicStream(musique);
    SetMusicVolume(musique, volume);

    // Boucle principale
    while (!WindowShouldClose())
    {
        // IMPORTANT
        UpdateMusicStream(musique);

        // Pause / Resume
        if (IsKeyPressed(KEY_P))
        {
            if (musiqueEnPause)
            {
                ResumeMusicStream(musique);
                musiqueEnPause = false;
            }
            else
            {
                PauseMusicStream(musique);
                musiqueEnPause = true;
            }
        }

        // Stop
        if (IsKeyPressed(KEY_S))
        {
            StopMusicStream(musique);
        }

        // Restart
        if (IsKeyPressed(KEY_R))
        {
            PlayMusicStream(musique);
        }

        // Volume +
        if (IsKeyPressed(KEY_UP))
        {
            volume += 0.1f;

            if (volume > 1.0f)
                volume = 1.0f;

            SetMusicVolume(musique, volume);
        }

        // Volume -
        if (IsKeyPressed(KEY_DOWN))
        {
            volume -= 0.1f;

            if (volume < 0.0f)
                volume = 0.0f;

            SetMusicVolume(musique, volume);
        }

        // Affichage
        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("MUSIQUE DE FOND", 250, 100, 30, BLACK);

        if (musiqueEnPause)
            DrawText("PAUSE", 350, 200, 25, ORANGE);
        else
            DrawText("EN COURS", 320, 200, 25, GREEN);

        DrawText(TextFormat("Volume : %.0f%%", volume * 100),
                 320, 280, 20, BLUE);

        DrawText("P : Pause / Resume", 280, 380, 20, DARKGRAY);
        DrawText("S : Stop", 280, 410, 20, DARKGRAY);
        DrawText("R : Restart", 280, 440, 20, DARKGRAY);
        DrawText("UP/DOWN : Volume", 280, 470, 20, DARKGRAY);
        

        EndDrawing();
    }

    // Nettoyage
    UnloadMusicStream(musique);

    CloseAudioDevice();
    CloseWindow();

    return 0;
}