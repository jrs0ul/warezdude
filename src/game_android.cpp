#include <jni.h>
#include <errno.h>

#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>

#include <disarray/VulkanVideo.h>
#include "Game.h"

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "native-activity", __VA_ARGS__))

static bool USE_VULKAN = true;

struct engine {
    struct android_app* app{};
    char ip[40];
    int animating{};
    EGLDisplay display{};
    EGLSurface surface{};
    EGLContext context{};
    EGLConfig config{};
    Vector3D oldDown;
    int32_t width{};
    int32_t height{};
    int32_t gamePosX{};
    VulkanVideo* vk{};
    Game* game{};
    bool loaded{};
    bool resetMovement{};
    bool backPressed{};

};


//-------------------------------------------
long getTicks()
{
    struct timespec now{};
    clock_gettime(CLOCK_MONOTONIC, &now);
    long msecs = now.tv_nsec / 1000000;
    return (long)now.tv_sec * 1000 + msecs;
}
//-------------------------------------------------
static int engine_init_display(struct engine* engine) {

    LOGI("initializing the display\n");

    if (!USE_VULKAN) {
        const EGLint attribs[] = {
                EGL_RENDERABLE_TYPE,
                EGL_OPENGL_ES2_BIT,
                EGL_SURFACE_TYPE,
                EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_DEPTH_SIZE, 24,
                EGL_NONE
        };

        EGLint w, h, format;
        EGLint numConfigs;
        EGLConfig config;
        EGLSurface surface;
        EGLContext context;
        EGLDisplay display;

        if (!engine->loaded)
        {
            LOGI("Let's make a new display\n");
            display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
            eglInitialize(display, 0, 0);
        } else
        {
            LOGI("Let's use an existing display\n");
            display = engine->display;
        }
        eglChooseConfig(display, attribs, &config, 1, &numConfigs);
        eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

        surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
        if (!engine->loaded)
        {
            LOGI("Let's create a new context\n");
            EGLint AttribList[] = {
                    EGL_CONTEXT_CLIENT_VERSION, 2,
                    EGL_NONE
            };
            context = eglCreateContext(display, config, NULL, AttribList);

        } else
        {
            LOGI("Let's use an existing context\n");
            context = engine->context;
        }

        if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE)
        {
            LOGW("Unable to eglMakeCurrent");
            return -1;
        }

        if (!engine->loaded)
        {
            eglQuerySurface(display, surface, EGL_WIDTH, &w);
            eglQuerySurface(display, surface, EGL_HEIGHT, &h);
            engine->width = w;
            engine->height = h;
        }

        engine->display = display;
        engine->context = context;
        engine->surface = surface;

        glDisable(GL_DEPTH_TEST);

        if (!engine->loaded)
        {
            if (engine->game)
            {
                engine->game->loadConfig();
                auto *sys = engine->game->getSysConfig();
                engine->gamePosX = 0;
                if (sys->ScreenWidth * sys->screenScaleX > engine->width) {
                    sys->screenScaleX = engine->width / sys->ScreenWidth;
                    sys->screenScaleY = sys->screenScaleX;
                    engine->game->screenHeight = sys->ScreenHeight * sys->screenScaleY;
                    engine->game->screenWidth = sys->ScreenWidth * sys->screenScaleX;
                }
                else
                {
                    engine->gamePosX = (int32_t)(engine->width / 2 - engine->game->screenWidth / 2);
                }

                engine->game->init(false);
                engine->game->timeTicks = (float) getTicks();
                engine->loaded = true;
                memset(engine->game->keys, 0, Game::GAME_KEY_COUNT);
            }
        }

        engine->animating = 1;
    }
    else // VULKAN
    {
        engine->vk = new VulkanVideo();
        engine->game->vk = engine->vk;

        engine->game->loadConfig();

        std::vector<const char *> extensions;
        extensions.push_back("VK_KHR_surface");
        extensions.push_back("VK_KHR_android_surface");
        if (VulkanVideo::USE_VALIDATION_LAYER)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        VkInstance* instance = engine->vk->createInstance((uint32_t)extensions.size(), extensions.data());

        uint32_t width = ANativeWindow_getWidth(engine->app->window);
        uint32_t height = ANativeWindow_getHeight(engine->app->window);
        VkSurfaceKHR  surface;
        const VkAndroidSurfaceCreateInfoKHR create_info{
                .sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR,
                .pNext = nullptr,
                .flags = 0,
                .window = engine->app->window};

        engine->width = (int32_t)width;
        engine->height = (int32_t)height;

        vkCreateAndroidSurfaceKHR(*instance, &create_info, nullptr, &surface);

        engine->gamePosX = (int32_t)(engine->width / 2 - engine->game->screenWidth / 2);

        if (!engine->vk->init(surface))
        {
            vkDestroySurfaceKHR(*instance, surface, nullptr);
            vkDestroyInstance(*instance, nullptr);
            return -1;
        }

        engine->game->init(USE_VULKAN);
        engine->game->timeTicks = (float) getTicks();
        engine->loaded = true;
        memset(engine->game->keys, 0, Game::GAME_KEY_COUNT);

        engine->animating = 1;
    }

    return 0;
}
//-----------------------------------------------------
//
static void engine_draw_frame(struct engine* engine)
{
    if (USE_VULKAN == false && engine->display == NULL)
    {
        return;
    }

    if (engine->game) {
        if (getTicks() > engine->game->tick) {

            engine->game->deltaTime = (getTicks() - engine->game->timeTicks) / 1000.0f;
            engine->game->timeTicks = getTicks();

            memcpy(engine->game->oldKeys, engine->game->keys, Game::GAME_KEY_COUNT);
            memset(engine->game->keys, 0, Game::GAME_KEY_COUNT);

            if (engine->backPressed)
            {
                engine->backPressed = false;
                engine->game->keys[5] = 1;
            }



            if (engine->game->touches->allfingersup)
            {
                engine->game->gamepadLAxis.x = 0;
                engine->game->gamepadLAxis.y = 0;
                engine->resetMovement = true;
            }

            if (!engine->game->touches->down.empty())
            {
                if (engine->resetMovement)
                {
                    engine->oldDown = engine->game->touches->down[0];
                    engine->resetMovement = false;
                }


                //if (engine->game->state != GAMESTATE_GAME)
                //{
                //    engine->game->MouseX = engine->game->touches.down[0].x / widthFactor;
                //    engine->game->MouseY = engine->game->touches.down[0].y / heightFactor;
                //}
            }

            if (!engine->game->touches->move.empty()) {

                Vector3D diff = engine->game->touches->move[0] - engine->oldDown;

                engine->game->gamepadLAxis.x = diff.x;
                engine->game->gamepadLAxis.y = diff.y;
                engine->game->gamepadRAxis.x = diff.x;
                engine->game->gamepadRAxis.y = diff.y;

                //if (engine->game->state != GAMESTATE_GAME)
                //{
                //    engine->game->MouseX = engine->game->touches.move[0].x / widthFactor;
                //    engine->game->MouseY = engine->game->touches.move[0].y / heightFactor;
                //}
            }

            engine->game->accumulator += engine->game->deltaTime;


            while (engine->game->accumulator >= engine->game->dT)
            {
                engine->game->logic();
                engine->game->accumulator -= engine->game->dT;
            }

            if (!USE_VULKAN) //opemgl
            {
                engine->game->renderToFBO(false);
                engine->game->renderFBO(false);
                eglSwapBuffers(engine->display, engine->surface);
            }
            else
            {
                engine->vk->getNextSwapImage();
                engine->vk->resetCommandBuffer();
                engine->vk->beginCommandBuffer();

                engine->game->renderToFBO(true);

                engine->vk->beginRenderPass({0.0f, 0.0f, 0.0f, 0.0f}, {1.0f, 0});

                engine->vk->setViewportAndScissor(engine->gamePosX, 0,
                                                  engine->game->screenWidth,
                                                  engine->game->screenHeight);
                engine->game->renderFBO(true);


                engine->vk->endRenderPass();
                engine->vk->endCommandBuffer();
                engine->vk->queueSubmit();
                engine->vk->queuePresent();
            }

            engine->game->tick = getTicks() + 1000/70;
        }




    }


}
//-------------------------------------------------------
/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine)
{
    if (!USE_VULKAN)
    {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }

        engine->surface = EGL_NO_SURFACE;
    }
    engine->animating = 0;

}
//-------------------------------------
/**
 * Process input.
 */
static int32_t engine_handle_input(struct android_app* app) {
    auto* engine = (struct engine*)app->userData;

    auto ib = android_app_swap_input_buffers(app);
    if (ib && ib->motionEventsCount)
    {
        for (int i = 0; i < ib->motionEventsCount; i++) {
            auto *event = &ib->motionEvents[i];
            int32_t ptrIdx = 0;

            const float widthFactor = (float)engine->game->screenWidth / (float)engine->game->getSysConfig()->ScreenWidth;
            const float heightFactor = (float)engine->game->screenHeight / (float)engine->game->getSysConfig()->ScreenHeight;

            switch (event->action & AMOTION_EVENT_ACTION_MASK) {

                case AMOTION_EVENT_ACTION_POINTER_DOWN:
                {
                    ptrIdx = (event->action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                            AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                    Vector3D v = Vector3D((GameActivityPointerAxes_getAxisValue(
                                                  &event->pointers[ptrIdx],
                                                  AMOTION_EVENT_AXIS_X)- engine->gamePosX) / widthFactor ,
                                          GameActivityPointerAxes_getAxisValue(
                                                  &event->pointers[ptrIdx],
                                                  AMOTION_EVENT_AXIS_Y) / heightFactor,
                                          0);
                    engine->game->touches->down.push_back(v);
                } break;
                case AMOTION_EVENT_ACTION_POINTER_UP:
                {
                    ptrIdx = (event->action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >>
                            AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
                    Vector3D v = Vector3D((GameActivityPointerAxes_getAxisValue(
                                                  &event->pointers[ptrIdx],
                                                  AMOTION_EVENT_AXIS_X) - engine->gamePosX)/ widthFactor,
                                          GameActivityPointerAxes_getAxisValue(
                                                  &event->pointers[ptrIdx],
                                                  AMOTION_EVENT_AXIS_Y) / heightFactor,
                                          0);
                    engine->game->touches->up.push_back(v);
                } break;


                case AMOTION_EVENT_ACTION_UP:
                {
                    engine->game->touches->allfingersup = true;

                    Vector3D v = Vector3D((GameActivityPointerAxes_getAxisValue(
                                                  &event->pointers[ptrIdx],
                                                  AMOTION_EVENT_AXIS_X) - engine->gamePosX) / widthFactor,
                                          GameActivityPointerAxes_getAxisValue(
                                                  &event->pointers[ptrIdx],
                                                  AMOTION_EVENT_AXIS_Y) / heightFactor,
                                          0);
                    engine->game->touches->up.push_back(v);
                }
                    break;
                case AMOTION_EVENT_ACTION_DOWN : {
                    engine->game->touches->allfingersup = false;
                    Vector3D v = Vector3D((GameActivityPointerAxes_getAxisValue(
                                                  &event->pointers[ptrIdx],
                                                  AMOTION_EVENT_AXIS_X) - engine->gamePosX) / widthFactor,
                                          GameActivityPointerAxes_getAxisValue(
                                                  &event->pointers[ptrIdx],
                                                  AMOTION_EVENT_AXIS_Y) / heightFactor,
                                          0);
                    engine->game->touches->down.push_back(v);
                }
                    break;
                case AMOTION_EVENT_ACTION_MOVE: {

                    engine->game->touches->allfingersup = false;
                    for (int j = 0; j < event->pointerCount; ++j)
                    {
                        Vector3D v = Vector3D((GameActivityPointerAxes_getAxisValue(
                                                      &event->pointers[j],
                                                      AMOTION_EVENT_AXIS_X) - engine->gamePosX)/ widthFactor,
                                              GameActivityPointerAxes_getAxisValue(
                                                      &event->pointers[j],
                                                      AMOTION_EVENT_AXIS_Y) /heightFactor,
                                              0);
                        engine->game->touches->move.push_back(v);
                    }

                }


            }
        }



        android_app_clear_motion_events(ib);
        return 1;
    }
    else if (ib && ib->keyEventsCount)
    {
        for (int i = 0; i < ib->keyEventsCount; i++)
        {
            auto *event = &ib->keyEvents[i];

            if (event->keyCode == AKEYCODE_BACK)
            {
                engine->backPressed = true;
            }

            if (event->action == AKEY_EVENT_ACTION_UP)
            {
                switch (event->keyCode)
                {

                    case AKEYCODE_DEL: {
                        int pos = strlen(engine->ip);
                        engine->ip[pos - 1] = 0;
                        engine->game->getIpEdit()->setText(engine->ip);
                        break;
                    }
                    case AKEYCODE_ENTER: {
                        GameActivity_hideSoftInput(engine->app->activity, 0);
                        engine->game->getIpEdit()->entered = true;
                        break;
                    }

                }

                if (event->keyCode != AKEYCODE_ENTER &&
                    event->keyCode != AKEYCODE_DEL &&
                    event->keyCode != AKEYCODE_BACK)
                {
                    char tmp[2] = {0};
                    tmp[0] = (char)event->unicodeChar;
                    strncat(engine->ip, tmp, 39);
                    engine->game->getIpEdit()->setText(engine->ip);
                }
            }
        }
        android_app_clear_key_events(ib);
        return 1;
    }

    return 0;
}
//----------------------------------
/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    auto* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            // The system has asked us to save our current state.  Do so.

            break;
        case APP_CMD_INIT_WINDOW:
            // The window is being shown, get it ready.
            if (engine->app->window != NULL)
            {
                engine->game->androidAssetManager = app->activity->assetManager;
                int res = engine_init_display(engine);
                if (res == -1)
                {
                    USE_VULKAN = false;
                    engine_init_display(engine);
                }
                engine_draw_frame(engine);

            }
            break;
        case APP_CMD_TERM_WINDOW:
            // The window is being hidden or closed, clean it up.
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS: engine->animating = 1; break;
        case APP_CMD_LOST_FOCUS: {
            engine->animating = 0;
            //engine->game->ss.stopMusic();
            engine_draw_frame(engine);
        }
            break;
        case APP_CMD_DESTROY:{

            LOGI("Let's destroy this app\n");
            if (engine->game) {
                engine->game->destroy();
            }

            if (engine->surface != EGL_NO_SURFACE) {
                LOGI("Let's destroy the surface\n");
                eglDestroySurface(engine->display, engine->surface);
            }

            if (engine->context != EGL_NO_CONTEXT) {
                LOGI("Let's destroy the context\n");
                eglDestroyContext(engine->display, engine->context);
            }



            eglTerminate(engine->display);

            delete engine->game;
        } break;
    }
}

extern "C" void GameTextInputGetStateCB(void *ctx, const struct GameTextInputState *state) {
    auto* engine = (struct engine*)ctx;
    if (!engine || !state) return;

    engine->game->getIpEdit()->setText(state->text_UTF8);


    // Clear the text input flag.
    engine->app->textInputState = 0;
}


//-------------------------------------------

void android_main(struct android_app* state) {
    struct engine engine;

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    engine.app = state;

    android_app_set_key_event_filter(state, NULL);
    android_app_set_motion_event_filter(state, NULL);


    ((struct engine*)(state->userData))->game = new Game();
    ((struct engine*)(state->userData))->loaded = false;


    // loop waiting for stuff to do.
    while (true) {
        int events;
        struct android_poll_source* source;

        while ((ALooper_pollOnce(engine.animating ? 0 : -1, nullptr, &events,
                                      (void**)&source)) >= 0) {

            // Process this event.
            if (source) {
                source->process(source->app, source);
            }

            // Check if we are exiting.
            if (state->destroyRequested) {
                engine_term_display(&engine);
                return;
            }
        }

        engine_handle_input(state);

        if (state->textInputState)
        {
            GameActivity_getTextInputState(
                    state->activity,
                    GameTextInputGetStateCB,
                    &engine
            );

        }


        if (engine.animating)
        {
            engine_draw_frame(&engine);
        }

        engine.game->network();

        if (engine.game->showTextInput)
        {
            GameActivity_showSoftInput(engine.app->activity, 2);
            engine.game->showTextInput = false;
        }
    }
}
