#include "local_context.hpp"

#include "global_context.hpp"

LocalContext::LocalContext() :
    context_(GlobalContext::instance().as_engine()->CreateContext()) {}

LocalContext::~LocalContext() {
    context_->Release();
    asThreadCleanup();
}

LocalContext&
LocalContext::instance() {
    static thread_local LocalContext local_ctx;
    return local_ctx;
}
