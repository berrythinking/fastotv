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

#include <string>

#include "client_server_types.h"
#include "serializer/json_serializer.h"

#define AUTH_INFO_LOGIN_FIELD "login"
#define AUTH_INFO_PASSWORD_FIELD "password"

namespace fasto {
namespace fastotv {

struct AuthInfo : public JsonSerializer<AuthInfo> {
  AuthInfo();
  AuthInfo(const std::string& login, const std::string& password);

  bool IsValid() const;

  common::Error Serialize(serialize_type* deserialized) const WARN_UNUSED_RESULT;
  static common::Error DeSerialize(const serialize_type& serialized,
                                   value_type* obj) WARN_UNUSED_RESULT;

  login_t login;  // unique
  std::string password;
};

inline bool operator==(const AuthInfo& lhs, const AuthInfo& rhs) {
  return lhs.login == rhs.login && lhs.password == rhs.password;
}

inline bool operator!=(const AuthInfo& x, const AuthInfo& y) {
  return !(x == y);
}

}  // namespace fastotv
}  // namespace fasto
