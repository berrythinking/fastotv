/*  Copyright (C) 2014-2018 FastoGT. All right reserved.

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

#include <memory>  // for shared_ptr
#include <string>  // for string

#include <json-c/json_object.h>  // for json_object

#include <common/error.h>                   // for Error
#include <common/libev/io_loop_observer.h>  // for IoLoopObserver
#include <common/libev/types.h>             // for timer_id_t
#include <common/macros.h>                  // for WARN_UNUSED_RESULT

#include "commands/commands.h"
#include "inner/inner_server_command_seq_parser.h"  // for InnerServerComman...

#include "server/config.h"  // for Config
#include "server/user_info.h"

#include "chat_message.h"

namespace common {
namespace libev {
class IoClient;
}
}  // namespace common
namespace common {
namespace libev {
class IoLoop;
}
}  // namespace common
namespace common {
namespace threads {
template <typename RT>
class Thread;
}
}  // namespace common

namespace fastotv {
namespace inner {
class InnerClient;
}
namespace server {
class UserStateInfo;
class ServerHost;
namespace redis {
class RedisPubSub;
}
namespace inner {

class InnerSubHandler;
class InnerTcpClient;

class InnerTcpHandlerHost : public fastotv::inner::InnerServerCommandSeqParser, public common::libev::IoLoopObserver {
 public:
  enum {
    ping_timeout_clients = 60,  // sec
    reread_cache_timeout = 150
  };

  explicit InnerTcpHandlerHost(ServerHost* parent, const Config& config);

  virtual void PreLooped(common::libev::IoLoop* server) override;

  virtual void Accepted(common::libev::IoClient* client) override;
  virtual void Moved(common::libev::IoLoop* server, common::libev::IoClient* client) override;
  virtual void Closed(common::libev::IoClient* client) override;

  virtual void DataReceived(common::libev::IoClient* client) override;
  virtual void DataReadyToWrite(common::libev::IoClient* client) override;
  virtual void PostLooped(common::libev::IoLoop* server) override;
  virtual void TimerEmited(common::libev::IoLoop* server, common::libev::timer_id_t id) override;
#if LIBEV_CHILD_ENABLE
  virtual void ChildStatusChanged(common::libev::IoLoop* server, pid_t id, int status) override;
#endif

  virtual ~InnerTcpHandlerHost();

  common::Error PublishToChannelOut(const std::string& msg);
  inner::InnerTcpClient* FindInnerConnectionByUserIDAndDeviceID(user_id_t user, device_id_t dev) const;

 private:
  void UpdateCache();

  void PublishUserStateInfo(const UserStateInfo& state);

  virtual void HandleInnerRequestCommand(fastotv::inner::InnerClient* connection,
                                         common::protocols::three_way_handshake::cmd_seq_t id,
                                         int argc,
                                         char* argv[]) override;
  virtual void HandleInnerResponceCommand(fastotv::inner::InnerClient* connection,
                                          common::protocols::three_way_handshake::cmd_seq_t id,
                                          int argc,
                                          char* argv[]) override;
  virtual void HandleInnerApproveCommand(fastotv::inner::InnerClient* connection,
                                         common::protocols::three_way_handshake::cmd_seq_t id,
                                         int argc,
                                         char* argv[]) override;

  // inner handlers
  common::Error HandleInnerSuccsessResponceCommand(fastotv::inner::InnerClient* connection,
                                                   common::protocols::three_way_handshake::cmd_seq_t id,
                                                   int argc,
                                                   char* argv[]) WARN_UNUSED_RESULT;
  common::Error HandleInnerFailedResponceCommand(fastotv::inner::InnerClient* connection,
                                                 common::protocols::three_way_handshake::cmd_seq_t id,
                                                 int argc,
                                                 char* argv[]) WARN_UNUSED_RESULT;

  common::Error ParserResponceResponceCommand(int argc, char* argv[], json_object** out) WARN_UNUSED_RESULT;

  void SendEnterChatMessage(common::libev::IoLoop* server, stream_id sid, login_t login);
  void SendLeaveChatMessage(common::libev::IoLoop* server, stream_id sid, login_t login);
  void BrodcastChatMessage(common::libev::IoLoop* server, const ChatMessage& msg);
  size_t GetOnlineUserByStreamId(common::libev::IoLoop* server, stream_id sid) const;

  ServerHost* const parent_;

  redis::RedisPubSub* sub_commands_in_;
  InnerSubHandler* handler_;
  std::shared_ptr<common::threads::Thread<void>> redis_subscribe_command_in_thread_;
  common::libev::timer_id_t ping_client_id_timer_;
  common::libev::timer_id_t reread_cache_id_timer_;
  const Config config_;

  mutable std::vector<stream_id> chat_channels_;
};

}  // namespace inner
}  // namespace server
}  // namespace fastotv
