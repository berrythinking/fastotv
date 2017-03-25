/*  Copyright (C) 2014-2017 FastoGT. All right reserved.

    This file is part of FastoTV.

    FastoTV is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FastoTV is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FastoTV. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <SDL2/SDL_render.h>

#include <common/smart_ptr.h>
#include <common/url.h>
#include <common/time.h>

#include "url.h"

#include "client/core/video_state_handler.h"
#include "client/core/app_options.h"
#include "client/core/events/events.h"

namespace fasto {
namespace fastotv {
namespace client {

namespace core {
class VideoState;
}

class NetworkController;

struct PlayerOptions {
  enum { width = 640, height = 480, volume = 100 };
  PlayerOptions();

  bool exit_on_keydown;
  bool exit_on_mousedown;
  bool is_full_screen;
  int default_width;
  int default_height;
  int screen_width;
  int screen_height;

  int audio_volume;  // Range: 0 - 100
  bool muted;
};

class Player : public core::VideoStateHandler {
 public:
  Player(const PlayerOptions& options,
         const core::AppOptions& opt,
         const core::ComplexOptions& copt);
  void SetFullScreen(bool full_screen);
  void SetMute(bool mute);
  ~Player();

 protected:
  virtual void HandleEvent(event_t* event) override;
  virtual void HandleExceptionEvent(event_t* event, common::Error err) override;

  virtual bool HandleRequestAudio(core::VideoState* stream,
                                  int64_t wanted_channel_layout,
                                  int wanted_nb_channels,
                                  int wanted_sample_rate,
                                  core::AudioParams* audio_hw_params) override;
  virtual void HanleAudioMix(uint8_t* audio_stream_ptr,
                             const uint8_t* src,
                             uint32_t len,
                             int volume) override;

  virtual bool HandleRealocFrame(core::VideoState* stream, core::VideoFrame* frame) override;
  virtual void HanleDisplayFrame(core::VideoState* stream, const core::VideoFrame* frame) override;
  virtual bool HandleRequestWindow(core::VideoState* stream) override;
  virtual void HandleDefaultWindowSize(int width, int height, AVRational sar) override;

  virtual void HandlePreExecEvent(core::events::PreExecEvent* event);
  virtual void HandlePostExecEvent(core::events::PostExecEvent* event);

  virtual void HandleTimerEvent(core::events::TimerEvent* event);

  virtual void HandleAllocFrameEvent(core::events::AllocFrameEvent* event);
  virtual void HandleQuitStreamEvent(core::events::QuitStreamEvent* event);

  virtual void HandleKeyPressEvent(core::events::KeyPressEvent* event);

  virtual void HandleWindowResizeEvent(core::events::WindowResizeEvent* event);
  virtual void HandleWindowExposeEvent(core::events::WindowExposeEvent* event);
  virtual void HandleWindowCloseEvent(core::events::WindowCloseEvent* event);

  virtual void HandleMousePressEvent(core::events::MousePressEvent* event);
  virtual void HandleMouseMoveEvent(core::events::MouseMoveEvent* event);

  virtual void HandleQuitEvent(core::events::QuitEvent* event);

  virtual void HandleClientConnectedEvent(core::events::ClientConnectedEvent* event);
  virtual void HandleClientDisconnectedEvent(core::events::ClientDisconnectedEvent* event);
  virtual void HandleClientConfigChangeEvent(core::events::ClientConfigChangeEvent* event);
  virtual void HandleReceiveChannelsEvent(core::events::ReceiveChannelsEvent* event);

 private:
  /* prepare a new audio buffer */
  static void sdl_audio_callback(void* opaque, uint8_t* stream, int len);

  void UpdateVolume(int step);
  int ReallocTexture(SDL_Texture** texture,
                     Uint32 new_format,
                     int new_width,
                     int new_height,
                     SDL_BlendMode blendmode,
                     bool init_texture);
  bool GetCurrentUrl(Url* url) const;
  void SwitchToErrorMode();
  void CalculateDispalySize();

  core::VideoState* CreateCurrentStream();
  core::VideoState* CreateNextStream();
  core::VideoState* CreatePrevStream();
  core::VideoState* CreateStreamPos(size_t pos);

  size_t GenerateNextPosition() const;
  size_t GeneratePrevPosition() const;

  PlayerOptions options_;
  const core::AppOptions opt_;
  const core::ComplexOptions copt_;
  channels_t play_list_;

  core::AudioParams* audio_params_;

  SDL_Renderer* renderer_;
  SDL_Window* window_;
  bool cursor_hidden_;
  core::msec_t cursor_last_shown_;
  core::msec_t last_mouse_left_click_;
  size_t curent_stream_pos_;

  SDL_Surface* surface_;
  core::VideoState* stream_;

  int width_;
  int height_;
  const int xleft_;
  const int ytop_;

  NetworkController* controller_;
};
}
}
}
