#pragma once

extern "C" {

void ai_resolve_asset_promise(int _id, const char* result);
void ai_resolve_fs_promise(int _id);
void ai_reject_promise(int _id, const char* msg);

}
