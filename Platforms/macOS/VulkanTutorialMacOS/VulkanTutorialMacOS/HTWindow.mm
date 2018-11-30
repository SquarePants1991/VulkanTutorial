//
// Created by ocean on 2018/10/18.
// Copyright (c) 2018 handytool. All rights reserved.
//

#include "HTWindow.h"
#include <Cocoa/Cocoa.h>
#include <MetalKit/MetalKit.h>

@interface HTWindowAppDelegate: NSObject <NSApplicationDelegate>
@end

@implementation HTWindowAppDelegate {
    HTWindow *_window;
    CVDisplayLinkRef _displayLink;
    double _lastLoopTime;
}

- (instancetype)initWithWindow:(HTWindow *)window {
    if (self = [super init]) {
        _window = window;
        [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(windowWillClose:) name:NSWindowWillCloseNotification object:nil];
        [self createLoop];
    }
    return self;
}

- (void)dealloc {
    if (_displayLink != NULL) {
        CVDisplayLinkRelease(_displayLink);
    }
}

static CVReturn renderCallback(CVDisplayLinkRef displayLink,
        const CVTimeStamp *inNow,
        const CVTimeStamp *inOutputTime,
        CVOptionFlags flagsIn,
        CVOptionFlags *flagsOut,
        void *displayLinkContext)
{
    [(__bridge HTWindowAppDelegate *)displayLinkContext appLoop:*inOutputTime];
    return kCVReturnSuccess;
}

- (void)createLoop {
    CGDirectDisplayID displayID = CGMainDisplayID();
    CVReturn error;
    error = CVDisplayLinkCreateWithCGDisplay(displayID, &_displayLink);
    if (!error) {
        CVDisplayLinkSetOutputCallback(_displayLink, renderCallback, (__bridge void *)self);
        _lastLoopTime = -1;
        CVDisplayLinkStart(_displayLink);
    } else {
        _displayLink = NULL;
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)notification {
    _window->launch();
}

- (void)windowWillClose:(NSNotification *)notification {
    [NSApp stop:self];
}

- (void)appLoop:(CVTimeStamp)time {
    if (_lastLoopTime < 0) {
        _lastLoopTime = CACurrentMediaTime();
        _window->deltaTime = 0;
        _window->elapsedTime = 0;
    } else {
        double now = CACurrentMediaTime();
        _window->deltaTime = now - _lastLoopTime;
        _window->elapsedTime += _window->deltaTime;
        _lastLoopTime = now;
    }
    _window->loop();
}
@end

HTWindow::HTWindow(int windowWidth, int windowHeight, const char *title):
        windowWidth(windowWidth),
        windowHeight(windowHeight) {
    [NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
    NSWindowStyleMask windowStyleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable;
    NSWindow *window = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, windowWidth, windowHeight)
                                                   styleMask:windowStyleMask backing:NSBackingStoreBuffered defer:NO];
    [window cascadeTopLeftFromPoint:NSMakePoint(windowWidth,windowHeight)];
    NSString *titleNSStr = [NSString stringWithUTF8String:title];
    [window setTitle:titleNSStr];
    [window makeKeyAndOrderFront:nil];
    MTKView *metalView = [MTKView.alloc initWithFrame:window.contentView.bounds];
    [window.contentView addSubview:metalView];
    [NSApp activateIgnoringOtherApps:YES];
    this->metalView = (__bridge void *)metalView;
    this->_window = (__bridge void *)window;
}

void HTWindow::present() {
    @autoreleasepool {
        HTWindowAppDelegate *appDelegate = [HTWindowAppDelegate.alloc initWithWindow:this];
        NSApplication * application = [NSApplication sharedApplication];
        [application setDelegate:appDelegate];
        [NSApp run];
    }
}

void HTWindow::setTitle(const char *title) {
    dispatch_async(dispatch_get_main_queue(), ^ {
        NSString *titleNSStr = [NSString stringWithUTF8String:title];
        [(__bridge NSWindow *)_window setTitle:titleNSStr];
    });
}
