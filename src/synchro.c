#include "synchro.h"
#include "ensitheora.h"
#include <pthread.h>


extern bool fini;

/* les variables pour la synchro, ici */

pthread_mutex_t mutex_1, mutex_2, mutex_3;
pthread_cond_t c_att, c_envoi, c_texture_att, c_texture_envoi;
pthread_cond_t c_producteur, c_consommateur;
int f_taille_pret = 0, f_texture_pret = 0, nb_texture = 0;


/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
        pthread_mutex_lock(&mutex_1);
        while (f_taille_pret == 1){
                pthread_cond_wait(&c_envoi, &mutex_1);
        }
        windowsx = buffer->width;
        windowsy = buffer->height;
        f_taille_pret = 1;
        pthread_cond_signal(&c_att);
        pthread_mutex_unlock(&mutex_1);
}

void attendreTailleFenetre() {
        pthread_mutex_lock(&mutex_1);
        while (f_taille_pret == 0){
                pthread_cond_wait(&c_att, &mutex_1);
        }
        f_taille_pret = 0;
        pthread_cond_signal(&c_envoi);
        pthread_mutex_unlock(&mutex_1);
}

void signalerFenetreEtTexturePrete() {
        pthread_mutex_lock(&mutex_2);
        while (f_texture_pret != 0){
                pthread_cond_wait(&c_texture_envoi, &mutex_2);
        }
        f_texture_pret = 1;
        pthread_cond_signal(&c_texture_att);
        pthread_mutex_unlock(&mutex_2);
}


void attendreFenetreTexture() {
        pthread_mutex_lock(&mutex_2);
        while (f_texture_pret == 0){
                pthread_cond_wait(&c_texture_att, &mutex_2);
        }
        f_texture_pret = 0;
        pthread_cond_signal(&c_texture_envoi);
        pthread_mutex_unlock(&mutex_2);
}

void debutConsommerTexture() {
        pthread_mutex_lock(&mutex_3);
        while(nb_texture == 0){
                pthread_cond_wait(&c_consommateur, &mutex_3);
        }
        pthread_mutex_unlock(&mutex_3);
}

void finConsommerTexture() {
        pthread_mutex_lock(&mutex_3);
        nb_texture --;
        if(nb_texture != NBTEX){
                pthread_cond_signal(&c_producteur);
        }
        pthread_mutex_unlock(&mutex_3);
}


void debutDeposerTexture() {
        pthread_mutex_lock(&mutex_3);
        while(nb_texture == NBTEX){
                pthread_cond_wait(&c_producteur, &mutex_3);
        }
        pthread_mutex_unlock(&mutex_3);
}

void finDeposerTexture() {
        pthread_mutex_lock(&mutex_3);
        nb_texture++;
        if(nb_texture != 0) {
                pthread_cond_signal(&c_consommateur);
        }
        pthread_mutex_unlock(&mutex_3);
}
