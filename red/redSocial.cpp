#pragma once

class Publicacion {
private:
    char postId[40];
    char userId[40];
    char postContent[256];
    char postDate[16];
    int likes;
    int comments;
    int shares;

    void copiarTexto(char* destino, const char* origen, int tamMax) {
        if (!destino || !origen || tamMax <= 0) return;
        int i = 0;
        while (origen[i] != '\0' && i < tamMax - 1) {
            destino[i] = origen[i];
            i++;
        }
        destino[i] = '\0';
    }

public:
    Publicacion() : likes(0), comments(0), shares(0) {
        postId[0] = '\0';
        userId[0] = '\0';
        postContent[0] = '\0';
        postDate[0] = '\0';
    }

    Publicacion(const char* pId, const char* uId, const char* content, const char* date, int l, int c, int s)
        : likes(l), comments(c), shares(s) {
        copiarTexto(postId, pId, 40);
        copiarTexto(userId, uId, 40);
        copiarTexto(postContent, content, 256);
        copiarTexto(postDate, date, 16);
    }

    const char* getPostId() const { return postId; }
    const char* getUserId() const { return userId; }
    const char* getPostContent() const { return postContent; }
    const char* getPostDate() const { return postDate; }
    int getLikes() const { return likes; }
    int getComments() const { return comments; }
    int getShares() const { return shares; }

    bool operator>(const Publicacion& otra) const { return likes > otra.likes; }
    bool operator<(const Publicacion& otra) const { return likes < otra.likes; }
};