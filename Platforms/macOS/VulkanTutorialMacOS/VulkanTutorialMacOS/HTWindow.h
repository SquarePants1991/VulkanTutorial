//
// Created by ocean on 2018/10/18.
// Copyright (c) 2018 handytool. All rights reserved.
//

#ifndef HTWindow_hpp
#define HTWindow_hpp

#import <cstdint>

class HTWindow {
    void *_window;
public:
    HTWindow(int windowWidth, int windowHeight, const char *title);
    int windowWidth;
    int windowHeight;
    void *metalView;
    double deltaTime;
    double elapsedTime;

    void present();
    void setTitle(const char *title);
    virtual void launch() {}
    virtual void loop() {}
};

#endif