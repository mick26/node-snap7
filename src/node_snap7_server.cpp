/*
 * Copyright (c) 2016, Mathias Küsel
 * MIT License <https://github.com/mathiask88/node-snap7/blob/master/LICENSE>
 */

#include <node_snap7_server.h>
#include <node_buffer.h>

namespace node_snap7 {

void S7API EventCallBack(void *usrPtr, PSrvEvent PEvent, int Size) {
  event_baton_g.SrvEvent = *PEvent;

  uv_async_send(&event_async_g);
}

int S7API RWAreaCallBack(void *usrPtr, int Sender, int Operation, PS7Tag PTag
  , void *pUsrData
) {
  rw_event_baton_g.Sender = Sender;
  rw_event_baton_g.Operation = Operation;
  rw_event_baton_g.Tag = *PTag;
  rw_event_baton_g.pUsrData = pUsrData;

  uv_async_send(&rw_async_g);

  uv_sem_wait(&sem);

  return 0;
}

Nan::Persistent<v8::FunctionTemplate> S7Server::constructor;

NAN_MODULE_INIT(S7Server::Init) {
  Nan::HandleScope scope;

  v8::Local<v8::FunctionTemplate> tpl;
  tpl = Nan::New<v8::FunctionTemplate>(S7Server::New);

  v8::Local<v8::String> name = Nan::New<v8::String>("S7Server")
    .ToLocalChecked();

  tpl->SetClassName(name);
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Setup the prototype
  Nan::SetPrototypeMethod(
    tpl
    , "Start"
    , S7Server::Start);
  Nan::SetPrototypeMethod(
    tpl
    , "StartTo"
    , S7Server::StartTo);
  Nan::SetPrototypeMethod(
    tpl
    , "Stop"
    , S7Server::Stop);
  Nan::SetPrototypeMethod(
    tpl
    , "SetResourceless"
    , S7Server::SetResourceless);
  Nan::SetPrototypeMethod(
    tpl
    , "RegisterArea"
    , S7Server::RegisterArea);
  Nan::SetPrototypeMethod(
    tpl
    , "UnregisterArea"
    , S7Server::UnregisterArea);
  Nan::SetPrototypeMethod(
    tpl
    , "LockArea"
    , S7Server::LockArea);
  Nan::SetPrototypeMethod(
    tpl
    , "UnlockArea"
    , S7Server::UnlockArea);
  Nan::SetPrototypeMethod(
    tpl
    , "SetArea"
    , S7Server::SetArea);
  Nan::SetPrototypeMethod(
    tpl
    , "GetArea"
    , S7Server::GetArea);

  // Error to text function
  Nan::SetPrototypeMethod(
    tpl
    , "ErrorText"
    , S7Server::ErrorText);

  // Event to text function
  Nan::SetPrototypeMethod(
    tpl
    , "EventText"
    , S7Server::EventText);

  // Error codes
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("errSrvCannotStart").ToLocalChecked()
    , Nan::New<v8::Integer>(errSrvCannotStart)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("errSrvDBNullPointer").ToLocalChecked()
    , Nan::New<v8::Integer>(errSrvDBNullPointer)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("errSrvAreaAlreadyExists").ToLocalChecked()
    , Nan::New<v8::Integer>(errSrvAreaAlreadyExists)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("errSrvUnknownArea").ToLocalChecked()
    , Nan::New<v8::Integer>(errSrvUnknownArea)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("errSrvInvalidParams").ToLocalChecked()
    , Nan::New<v8::Integer>(errSrvInvalidParams)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("errSrvTooManyDB").ToLocalChecked()
    , Nan::New<v8::Integer>(errSrvTooManyDB)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("errSrvInvalidParamNumber").ToLocalChecked()
    , Nan::New<v8::Integer>(errSrvInvalidParamNumber)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("errSrvCannotChangeParam").ToLocalChecked()
    , Nan::New<v8::Integer>(errSrvCannotChangeParam)
    , v8::ReadOnly);

  // Server Area IDs
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("srvAreaPE").ToLocalChecked()
    , Nan::New<v8::Integer>(srvAreaPE)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("srvAreaPA").ToLocalChecked()
    , Nan::New<v8::Integer>(srvAreaPA)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("srvAreaMK").ToLocalChecked()
    , Nan::New<v8::Integer>(srvAreaMK)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("srvAreaCT").ToLocalChecked()
    , Nan::New<v8::Integer>(srvAreaCT)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("srvAreaTM").ToLocalChecked()
    , Nan::New<v8::Integer>(srvAreaTM)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("srvAreaDB").ToLocalChecked()
    , Nan::New<v8::Integer>(srvAreaDB)
    , v8::ReadOnly);

  // TCP Server Event codes
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcServerStarted").ToLocalChecked()
    , Nan::New<v8::Integer>(evcServerStarted)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcServerStopped").ToLocalChecked()
    , Nan::New<v8::Integer>(evcServerStopped)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcListenerCannotStart").ToLocalChecked()
    , Nan::New<v8::Integer>(evcListenerCannotStart)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcClientAdded").ToLocalChecked()
    , Nan::New<v8::Integer>(evcClientAdded)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcClientRejected").ToLocalChecked()
    , Nan::New<v8::Integer>(evcClientRejected)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcClientNoRoom").ToLocalChecked()
    , Nan::New<v8::Integer>(evcClientNoRoom)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcClientException").ToLocalChecked()
    , Nan::New<v8::Integer>(evcClientException)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcClientDisconnected").ToLocalChecked()
    , Nan::New<v8::Integer>(evcClientDisconnected)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcClientTerminated").ToLocalChecked()
    , Nan::New<v8::Integer>(evcClientTerminated)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcClientsDropped").ToLocalChecked()
    , Nan::New<v8::Integer>(evcClientsDropped)
    , v8::ReadOnly);

  // S7 Server Event Codes
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcPDUincoming").ToLocalChecked()
    , Nan::New<v8::Integer>(evcPDUincoming)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcDataRead").ToLocalChecked()
    , Nan::New<v8::Integer>(evcDataRead)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcDataWrite").ToLocalChecked()
    , Nan::New<v8::Integer>(evcDataWrite)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcNegotiatePDU").ToLocalChecked()
    , Nan::New<v8::Integer>(evcNegotiatePDU)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcReadSZL").ToLocalChecked()
    , Nan::New<v8::Integer>(evcReadSZL)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcClock").ToLocalChecked()
    , Nan::New<v8::Integer>(evcClock)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcUpload").ToLocalChecked()
    , Nan::New<v8::Integer>(evcUpload)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcDownload").ToLocalChecked()
    , Nan::New<v8::Integer>(evcDownload)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcDirectory").ToLocalChecked()
    , Nan::New<v8::Integer>(evcDirectory)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcSecurity").ToLocalChecked()
    , Nan::New<v8::Integer>(evcSecurity)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcControl").ToLocalChecked()
    , Nan::New<v8::Integer>(evcControl)
    , v8::ReadOnly);

  // Masks to enable/disable all events
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcAll").ToLocalChecked()
    , Nan::New<v8::Integer>(evcAll)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evcNone").ToLocalChecked()
    , Nan::New<v8::Integer>(evcNone)
    , v8::ReadOnly);

  // Event SubCodes
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evsUnknown").ToLocalChecked()
    , Nan::New<v8::Integer>(evsUnknown)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evsStartUpload").ToLocalChecked()
    , Nan::New<v8::Integer>(evsStartUpload)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evsStartDownload").ToLocalChecked()
    , Nan::New<v8::Integer>(evsStartDownload)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evsGetBlockList").ToLocalChecked()
    , Nan::New<v8::Integer>(evsGetBlockList)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evsStartListBoT").ToLocalChecked()
    , Nan::New<v8::Integer>(evsStartListBoT)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evsListBoT").ToLocalChecked()
    , Nan::New<v8::Integer>(evsListBoT)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evsGetBlockInfo").ToLocalChecked()
    , Nan::New<v8::Integer>(evsGetBlockInfo)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evsGetClock").ToLocalChecked()
    , Nan::New<v8::Integer>(evsGetClock)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evsSetClock").ToLocalChecked()
    , Nan::New<v8::Integer>(evsSetClock)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evsSetPassword").ToLocalChecked()
    , Nan::New<v8::Integer>(evsSetPassword)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evsClrPassword").ToLocalChecked()
    , Nan::New<v8::Integer>(evsClrPassword)
    , v8::ReadOnly);

  // Event Params : functions group
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("grProgrammer").ToLocalChecked()
    , Nan::New<v8::Integer>(grProgrammer)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("grCyclicData").ToLocalChecked()
    , Nan::New<v8::Integer>(grCyclicData)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("grBlocksInfo").ToLocalChecked()
    , Nan::New<v8::Integer>(grBlocksInfo)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("grSZL").ToLocalChecked()
    , Nan::New<v8::Integer>(grSZL)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("grPassword").ToLocalChecked()
    , Nan::New<v8::Integer>(grPassword)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("grBSend").ToLocalChecked()
    , Nan::New<v8::Integer>(grBSend)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("grClock").ToLocalChecked()
    , Nan::New<v8::Integer>(grClock)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("grSecurity").ToLocalChecked()
    , Nan::New<v8::Integer>(grSecurity)
    , v8::ReadOnly);

  // Event Params : control codes
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("CodeControlUnknown").ToLocalChecked()
    , Nan::New<v8::Integer>(CodeControlUnknown)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("CodeControlColdStart").ToLocalChecked()
    , Nan::New<v8::Integer>(CodeControlColdStart)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("CodeControlWarmStart").ToLocalChecked()
    , Nan::New<v8::Integer>(CodeControlWarmStart)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("CodeControlStop").ToLocalChecked()
    , Nan::New<v8::Integer>(CodeControlStop)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("CodeControlCompress").ToLocalChecked()
    , Nan::New<v8::Integer>(CodeControlCompress)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("CodeControlCpyRamRom").ToLocalChecked()
    , Nan::New<v8::Integer>(CodeControlCpyRamRom)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("CodeControlInsDel").ToLocalChecked()
    , Nan::New<v8::Integer>(CodeControlInsDel)
    , v8::ReadOnly);

  // Event Result
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrNoError").ToLocalChecked()
    , Nan::New<v8::Integer>(evrNoError)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrFragmentRejected").ToLocalChecked()
    , Nan::New<v8::Integer>(evrFragmentRejected)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrMalformedPDU").ToLocalChecked()
    , Nan::New<v8::Integer>(evrMalformedPDU)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrSparseBytes").ToLocalChecked()
    , Nan::New<v8::Integer>(evrSparseBytes)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrCannotHandlePDU").ToLocalChecked()
    , Nan::New<v8::Integer>(evrCannotHandlePDU)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrNotImplemented").ToLocalChecked()
    , Nan::New<v8::Integer>(evrNotImplemented)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrErrException").ToLocalChecked()
    , Nan::New<v8::Integer>(evrErrException)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrErrAreaNotFound").ToLocalChecked()
    , Nan::New<v8::Integer>(evrErrAreaNotFound)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrErrOutOfRange").ToLocalChecked()
    , Nan::New<v8::Integer>(evrErrOutOfRange)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrErrOverPDU").ToLocalChecked()
    , Nan::New<v8::Integer>(evrErrOverPDU)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrErrTransportSize").ToLocalChecked()
    , Nan::New<v8::Integer>(evrErrTransportSize)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrInvalidGroupUData").ToLocalChecked()
    , Nan::New<v8::Integer>(evrInvalidGroupUData)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrInvalidSZL").ToLocalChecked()
    , Nan::New<v8::Integer>(evrInvalidSZL)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrDataSizeMismatch").ToLocalChecked()
    , Nan::New<v8::Integer>(evrDataSizeMismatch)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrCannotUpload").ToLocalChecked()
    , Nan::New<v8::Integer>(evrCannotUpload)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrCannotDownload").ToLocalChecked()
    , Nan::New<v8::Integer>(evrCannotDownload)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrUploadInvalidID").ToLocalChecked()
    , Nan::New<v8::Integer>(evrUploadInvalidID)
    , v8::ReadOnly);
  Nan::SetPrototypeTemplate(
    tpl
    , Nan::New<v8::String>("evrResNotFound").ToLocalChecked()
    , Nan::New<v8::Integer>(evrResNotFound)
    , v8::ReadOnly);

  constructor.Reset(tpl);
  Nan::Set(target, name, tpl->GetFunction());
}

NAN_METHOD(S7Server::New) {
  if (info.IsConstructCall()) {
    S7Server *s7Server = new S7Server();

    s7Server->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
  } else {
    v8::Local<v8::FunctionTemplate> constructorHandle;
    constructorHandle = Nan::New<v8::FunctionTemplate>(constructor);
    info.GetReturnValue().Set(
      Nan::NewInstance(constructorHandle->GetFunction()).ToLocalChecked());
  }
}

S7Server::S7Server() {
  snap7Server = new TS7Server();

  event_async_g.data = rw_async_g.data = this;

  uv_async_init(uv_default_loop(), &event_async_g, S7Server::HandleEvent);
  uv_async_init(uv_default_loop(), &rw_async_g, S7Server::HandleReadWriteEvent);

  uv_unref(reinterpret_cast<uv_handle_t *>(&event_async_g));
  uv_unref(reinterpret_cast<uv_handle_t *>(&rw_async_g));
  uv_mutex_init(&mutex);
  uv_sem_init(&sem, 0);

  snap7Server->SetEventsCallback(&EventCallBack, NULL);
}

S7Server::~S7Server() {
  snap7Server->Stop();
  delete snap7Server;

  constructor.Reset();

  uv_close(reinterpret_cast<uv_handle_t *>(&event_async_g), 0);
  uv_close(reinterpret_cast<uv_handle_t *>(&rw_async_g), 0);
  uv_sem_destroy(&sem);
  uv_mutex_destroy(&mutex);
}

int S7Server::GetByteCountFromWordLen(int WordLen) {
  switch (WordLen) {
  case S7WLBit:
  case S7WLByte:
    return 1;
  case S7WLWord:
  case S7WLCounter:
  case S7WLTimer:
    return 2;
  case S7WLReal:
  case S7WLDWord:
    return 4;
  default:
    return 0;
  }
}

NAN_METHOD(S7Server::RWBufferCallback) {
  Nan::HandleScope scope;

  if (!node::Buffer::HasInstance(info[0])) {
    return Nan::ThrowTypeError("Wrong argument");
  } else {
    int byteCount, size;
    byteCount = S7Server::GetByteCountFromWordLen(rw_event_baton_g.Tag.WordLen);
    size = byteCount * rw_event_baton_g.Tag.Size;

    if (node::Buffer::Length(info[0]) < size) {
      return Nan::ThrowTypeError("Buffer length too small");
    } else {
      memcpy(
          rw_event_baton_g.pUsrData
        , node::Buffer::Data(info[0].As<v8::Object>())
        , size);
    }
  }
  uv_sem_post(&sem);
}

#if NODE_VERSION_AT_LEAST(0, 11, 13)
void S7Server::HandleEvent(uv_async_t* handle) {
#else
void S7Server::HandleEvent(uv_async_t* handle, int status) {
#endif
  Nan::HandleScope scope;

  S7Server *s7server = static_cast<S7Server*>(handle->data);
  in_addr sin;
  sin.s_addr = event_baton_g.SrvEvent.EvtSender;
  double time = static_cast<double>(event_baton_g.SrvEvent.EvtTime * 1000);

  v8::Local<v8::Object> event_obj = Nan::New<v8::Object>();
  Nan::Set(event_obj, Nan::New<v8::String>("EvtTime").ToLocalChecked()
    , Nan::New<v8::Date>(time).ToLocalChecked());
  Nan::Set(event_obj, Nan::New<v8::String>("EvtSender").ToLocalChecked()
    , Nan::New<v8::String>(inet_ntoa(sin)).ToLocalChecked());
  Nan::Set(event_obj, Nan::New<v8::String>("EvtCode").ToLocalChecked()
    , Nan::New<v8::Integer>(event_baton_g.SrvEvent.EvtCode));
  Nan::Set(event_obj, Nan::New<v8::String>("EvtRetCode").ToLocalChecked()
    , Nan::New<v8::Integer>(event_baton_g.SrvEvent.EvtRetCode));
  Nan::Set(event_obj, Nan::New<v8::String>("EvtParam1").ToLocalChecked()
    , Nan::New<v8::Integer>(event_baton_g.SrvEvent.EvtParam1));
  Nan::Set(event_obj, Nan::New<v8::String>("EvtParam2").ToLocalChecked()
    , Nan::New<v8::Integer>(event_baton_g.SrvEvent.EvtParam2));
  Nan::Set(event_obj, Nan::New<v8::String>("EvtParam3").ToLocalChecked()
    , Nan::New<v8::Integer>(event_baton_g.SrvEvent.EvtParam3));
  Nan::Set(event_obj, Nan::New<v8::String>("EvtParam4").ToLocalChecked()
    , Nan::New<v8::Integer>(event_baton_g.SrvEvent.EvtParam4));

  v8::Local<v8::Value> argv[2] = {
    Nan::New("event").ToLocalChecked(),
    event_obj
  };

  Nan::MakeCallback(s7server->handle(), "emit", 2, argv);
}

#if NODE_VERSION_AT_LEAST(0, 11, 13)
void S7Server::HandleReadWriteEvent(uv_async_t* handle) {
#else
void S7Server::HandleReadWriteEvent(uv_async_t* handle, int status) {
#endif
  Nan::HandleScope scope;

  S7Server *s7server = static_cast<S7Server*>(handle->data);
  in_addr sin;
  sin.s_addr = rw_event_baton_g.Sender;

  v8::Local<v8::Object> rw_tag_obj = Nan::New<v8::Object>();
  Nan::Set(rw_tag_obj, Nan::New<v8::String>("Area").ToLocalChecked()
    , Nan::New<v8::Integer>(rw_event_baton_g.Tag.Area));
  Nan::Set(rw_tag_obj, Nan::New<v8::String>("DBNumber").ToLocalChecked()
    , Nan::New<v8::Integer>(rw_event_baton_g.Tag.DBNumber));
  Nan::Set(rw_tag_obj, Nan::New<v8::String>("Start").ToLocalChecked()
    , Nan::New<v8::Integer>(rw_event_baton_g.Tag.Start));
  Nan::Set(rw_tag_obj, Nan::New<v8::String>("Size").ToLocalChecked()
    , Nan::New<v8::Integer>(rw_event_baton_g.Tag.Size));
  Nan::Set(rw_tag_obj, Nan::New<v8::String>("WordLen").ToLocalChecked()
    , Nan::New<v8::Integer>(rw_event_baton_g.Tag.WordLen));

  int byteCount, size;
  byteCount = S7Server::GetByteCountFromWordLen(rw_event_baton_g.Tag.WordLen);
  size = byteCount * rw_event_baton_g.Tag.Size;

  v8::Local<v8::Object> buffer;

  if (rw_event_baton_g.Operation == OperationWrite) {
    buffer = Nan::CopyBuffer(
      static_cast<char*>(rw_event_baton_g.pUsrData),
      size).ToLocalChecked();
  } else {
    buffer = Nan::NewBuffer(size).ToLocalChecked();
    memset(node::Buffer::Data(buffer), 0, size);
  }

  v8::Local<v8::Value> argv[6] = {
    Nan::New("readWrite").ToLocalChecked(),
    Nan::New<v8::String>(inet_ntoa(sin)).ToLocalChecked(),
    Nan::New<v8::Integer>(rw_event_baton_g.Operation),
    rw_tag_obj,
    buffer,
    Nan::New<v8::Function>(S7Server::RWBufferCallback)
  };

  Nan::MakeCallback(s7server->handle(), "emit", 6, argv);
}

void IOWorkerServer::Execute() {
  uv_mutex_lock(&s7server->mutex);

  switch (caller) {
  case STARTTO:
    returnValue = s7server->snap7Server->StartTo(
      **static_cast<Nan::Utf8String*>(pData));

    if (returnValue == 0)
      uv_ref(reinterpret_cast<uv_handle_t *>(&event_async_g));
    break;

  case START:
    returnValue = s7server->snap7Server->Start();

    if (returnValue == 0)
      uv_ref(reinterpret_cast<uv_handle_t *>(&event_async_g));
    break;

  case STOP:
    returnValue = s7server->snap7Server->Stop();

    if (returnValue == 0)
      uv_unref(reinterpret_cast<uv_handle_t *>(&event_async_g));
    break;
  }

  uv_mutex_unlock(&s7server->mutex);
}

void IOWorkerServer::HandleOKCallback() {
  Nan::HandleScope scope;

  v8::Local<v8::Value> argv1[1];
  v8::Local<v8::Value> argv2[2];

  if (returnValue == 0) {
    argv2[0] = argv1[0] = Nan::Null();
  } else {
    argv2[0] = argv1[0] = Nan::New<v8::Integer>(returnValue);
  }

  switch (caller) {
  case STARTTO:
    delete static_cast<Nan::Utf8String*>(pData);
    callback->Call(1, argv1);
    break;

  case START:
  case STOP:
    callback->Call(1, argv1);
    break;
  }
}

NAN_METHOD(S7Server::Start) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsFunction()) {
    int ret = s7server->snap7Server->Start();

    if (ret == 0)
      uv_ref(reinterpret_cast<uv_handle_t *>(&event_async_g));

    info.GetReturnValue().Set(Nan::New<v8::Boolean>(ret == 0));
  } else {
    Nan::Callback *callback = new Nan::Callback(info[0].As<v8::Function>());
    Nan::AsyncQueueWorker(new IOWorkerServer(callback, s7server, START));
    info.GetReturnValue().SetUndefined();
  }
}

NAN_METHOD(S7Server::StartTo) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (info.Length() < 1) {
    return Nan::ThrowTypeError("Wrong number of arguments");
  }

  if (!info[0]->IsString()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  Nan::Utf8String *address = new Nan::Utf8String(info[0]);
  if (!info[1]->IsFunction()) {
    int ret = s7server->snap7Server->StartTo(**address);
    delete address;

    if (ret == 0)
      uv_ref(reinterpret_cast<uv_handle_t *>(&event_async_g));

    info.GetReturnValue().Set(Nan::New<v8::Boolean>(ret == 0));
  } else {
    Nan::Callback *callback = new Nan::Callback(info[1].As<v8::Function>());
    Nan::AsyncQueueWorker(new IOWorkerServer(callback, s7server, STARTTO
      , address));
    info.GetReturnValue().SetUndefined();
  }
}

NAN_METHOD(S7Server::Stop) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsFunction()) {
    int ret = s7server->snap7Server->Stop();

    if (ret == 0)
      uv_unref(reinterpret_cast<uv_handle_t *>(&event_async_g));

    info.GetReturnValue().Set(Nan::New<v8::Boolean>(ret == 0));
  } else {
    Nan::Callback *callback = new Nan::Callback(info[0].As<v8::Function>());
    Nan::AsyncQueueWorker(new IOWorkerServer(callback, s7server, STOP));
    info.GetReturnValue().SetUndefined();
  }
}

NAN_METHOD(S7Server::SetResourceless) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsBoolean()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  bool resourceless = info[0]->BooleanValue();

  if (resourceless) {
    s7server->snap7Server->SetRWAreaCallback(&RWAreaCallBack, NULL);
  } else {
    s7server->snap7Server->SetRWAreaCallback(NULL, NULL);
  }

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(S7Server::GetParam) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  int pData;
  int returnValue = s7server->snap7Server->GetParam(info[0]->Int32Value()
    , &pData);

  if (returnValue == 0) {
    info.GetReturnValue().Set(Nan::New<v8::Integer>(pData));
  } else {
    info.GetReturnValue().Set(Nan::New<v8::Integer>(returnValue));
  }
}

NAN_METHOD(S7Server::SetParam) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!(info[0]->IsInt32() || info[1]->IsInt32())) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  int pData = info[1]->Int32Value();
  int ret = s7server->snap7Server->SetParam(info[0]->Int32Value(), &pData);
  info.GetReturnValue().Set(Nan::New<v8::Boolean>(ret == 0));
}

NAN_METHOD(S7Server::GetEventsMask) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  int returnValue = s7server->snap7Server->GetEventsMask();
  if (returnValue == 0) {
    info.GetReturnValue().Set(Nan::False());
  } else {
    info.GetReturnValue().Set(Nan::New<v8::Integer>(returnValue));
  }
}

NAN_METHOD(S7Server::SetEventsMask) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  s7server->snap7Server->SetEventsMask(info[0]->Int32Value());
  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(S7Server::RegisterArea) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  int index;
  char *pBuffer;
  size_t len;
  int area = info[0]->Int32Value();

  if (area == srvAreaDB) {
    if (!info[1]->IsInt32() || !node::Buffer::HasInstance(info[2])) {
      return Nan::ThrowTypeError("Wrong arguments");
    }

    index = info[1]->Int32Value();
    len = node::Buffer::Length(info[2]);
    pBuffer = node::Buffer::Data(info[2]);
  } else if (!node::Buffer::HasInstance(info[1])) {
    return Nan::ThrowTypeError("Wrong arguments");
  } else {
    index = 0;
    len = node::Buffer::Length(info[1]);
    pBuffer = node::Buffer::Data(info[1]);
  }

  if (len > 0xFFFF) {
    return Nan::ThrowRangeError("Max area buffer size is 65535");
  }

  word size = static_cast<word>(len);
  char *data = new char[size];
  memcpy(data, pBuffer, size);

  int ret = s7server->snap7Server->RegisterArea(area, index, data, size);

  if (ret == 0) {
    s7server->area2buffer[area][index].pBuffer = data;
    s7server->area2buffer[area][index].size = size;
  } else {
    delete data;
  }

  info.GetReturnValue().Set(Nan::New<v8::Boolean>(ret == 0));
}

NAN_METHOD(S7Server::UnregisterArea) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  int index = 0;
  int area = info[0]->Int32Value();

  if (area == srvAreaDB) {
    if (!info[1]->IsInt32()) {
      return Nan::ThrowTypeError("Wrong arguments");
    }

    index = info[1]->Int32Value();
  }

  int ret = s7server->snap7Server->UnregisterArea(area, index);

  if (ret == 0) {
    delete s7server->area2buffer[area][index].pBuffer;
    s7server->area2buffer.erase(area);
  }

  info.GetReturnValue().Set(Nan::New<v8::Boolean>(ret == 0));
}

NAN_METHOD(S7Server::SetArea) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  int area = info[0]->Int32Value();
  if (!s7server->area2buffer.count(area)) {
    return Nan::ThrowError("Area not found");
  }

  int index;
  char *pBuffer;
  size_t len;

  if (area == srvAreaDB) {
    if (!info[1]->IsInt32() || !node::Buffer::HasInstance(info[2])) {
      return Nan::ThrowTypeError("Wrong arguments");
    }

    index = info[1]->Int32Value();
    if (!s7server->area2buffer[area].count(index)) {
      return Nan::ThrowError("DB index not found");
    }

    len = node::Buffer::Length(info[2]);
    pBuffer = node::Buffer::Data(info[2]);
  } else if (!node::Buffer::HasInstance(info[1])) {
    return Nan::ThrowTypeError("Wrong arguments");
  } else {
    index = 0;
    len = node::Buffer::Length(info[1]);
    pBuffer = node::Buffer::Data(info[1]);
  }

  if (len != s7server->area2buffer[area][index].size) {
    return Nan::ThrowError("Wrong buffer length");
  }

  s7server->snap7Server->LockArea(area, index);

  memcpy(
    s7server->area2buffer[area][index].pBuffer
    , pBuffer, s7server->area2buffer[area][index].size);

  s7server->snap7Server->UnlockArea(area, index);

  info.GetReturnValue().SetUndefined();
}

NAN_METHOD(S7Server::GetArea) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  int index = 0;
  int area = info[0]->Int32Value();

  if (!s7server->area2buffer.count(area)) {
    return Nan::ThrowError("Area not found");
  }

  if (area == srvAreaDB) {
    if (!info[1]->IsInt32()) {
      return Nan::ThrowTypeError("Wrong arguments");
    }

    if (!s7server->area2buffer[area].count(index)) {
      return Nan::ThrowError("DB index not found");
    }

    index = info[1]->Int32Value();
  }

  s7server->snap7Server->LockArea(area, index);

  v8::Local<v8::Object> buffer = Nan::CopyBuffer(
      s7server->area2buffer[area][index].pBuffer
    , s7server->area2buffer[area][index].size).ToLocalChecked();

  s7server->snap7Server->UnlockArea(area, index);

  info.GetReturnValue().Set(buffer);
}

NAN_METHOD(S7Server::LockArea) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  int index = 0;
  int area = info[0]->Int32Value();

  if (area == srvAreaDB) {
    if (!info[1]->IsInt32()) {
      return Nan::ThrowTypeError("Wrong arguments");
    }

    index = info[1]->Int32Value();
  }

  int ret = s7server->snap7Server->LockArea(area, index);

  info.GetReturnValue().Set(Nan::New<v8::Boolean>(ret == 0));
}

NAN_METHOD(S7Server::UnlockArea) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  int index = 0;
  int area = info[0]->Int32Value();

  if (area == srvAreaDB) {
    if (!info[1]->IsInt32()) {
      return Nan::ThrowTypeError("Wrong arguments");
    }

    index = info[1]->Int32Value();
  }

  int ret = s7server->snap7Server->UnlockArea(area, index);

  info.GetReturnValue().Set(Nan::New<v8::Boolean>(ret == 0));
}

NAN_METHOD(S7Server::ServerStatus) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  int returnValue = s7server->snap7Server->ServerStatus();
  if ((returnValue == 0) || (returnValue == 1) || (returnValue == 2)) {
    info.GetReturnValue().Set(Nan::New<v8::Integer>(returnValue));
  } else {
    info.GetReturnValue().Set(Nan::False());
  }
}

NAN_METHOD(S7Server::ClientsCount) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  int returnValue = s7server->snap7Server->ClientsCount();
  info.GetReturnValue().Set(Nan::New<v8::Integer>(returnValue));
}

NAN_METHOD(S7Server::GetCpuStatus) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  int returnValue = s7server->snap7Server->GetCpuStatus();
  if ((returnValue == S7CpuStatusUnknown) ||
    (returnValue == S7CpuStatusStop) ||
    (returnValue == S7CpuStatusRun)) {
    info.GetReturnValue().Set(Nan::New<v8::Integer>(returnValue));
  } else {
    info.GetReturnValue().Set(Nan::False());
  }
}

NAN_METHOD(S7Server::SetCpuStatus) {
  S7Server *s7server = ObjectWrap::Unwrap<S7Server>(info.Holder());

  if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  int ret = s7server->snap7Server->SetCpuStatus(info[0]->Int32Value());
  info.GetReturnValue().Set(Nan::New<v8::Boolean>(ret == 0));
}

NAN_METHOD(S7Server::ErrorText) {
  if (!info[0]->IsInt32()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  info.GetReturnValue().Set(Nan::New<v8::String>(
    SrvErrorText(info[0]->Int32Value()).c_str()).ToLocalChecked());
}

NAN_METHOD(S7Server::EventText) {
  TSrvEvent SrvEvent;

  if (!info[0]->IsObject()) {
    return Nan::ThrowTypeError("Wrong arguments");
  }

  v8::Local<v8::Object> event_obj = v8::Local<v8::Object>::Cast(info[0]);
  if (!Nan::Has(event_obj, Nan::New<v8::String>("EvtTime").ToLocalChecked()).FromJust() ||
    !Nan::Has(event_obj, Nan::New<v8::String>("EvtSender").ToLocalChecked()).FromJust() ||
    !Nan::Has(event_obj, Nan::New<v8::String>("EvtCode").ToLocalChecked()).FromJust() ||
    !Nan::Has(event_obj, Nan::New<v8::String>("EvtRetCode").ToLocalChecked()).FromJust() ||
    !Nan::Has(event_obj, Nan::New<v8::String>("EvtParam1").ToLocalChecked()).FromJust() ||
    !Nan::Has(event_obj, Nan::New<v8::String>("EvtParam2").ToLocalChecked()).FromJust() ||
    !Nan::Has(event_obj, Nan::New<v8::String>("EvtParam3").ToLocalChecked()).FromJust() ||
    !Nan::Has(event_obj, Nan::New<v8::String>("EvtParam4").ToLocalChecked()).FromJust()) {
    return Nan::ThrowTypeError("Wrong argument structure");
  }

  if (!Nan::Get(event_obj, Nan::New<v8::String>("EvtTime").ToLocalChecked()).ToLocalChecked()->IsDate() ||
    !Nan::Get(event_obj, Nan::New<v8::String>("EvtSender").ToLocalChecked()).ToLocalChecked()->IsString() ||
    !Nan::Get(event_obj, Nan::New<v8::String>("EvtCode").ToLocalChecked()).ToLocalChecked()->IsInt32() ||
    !Nan::Get(event_obj, Nan::New<v8::String>("EvtRetCode").ToLocalChecked()).ToLocalChecked()->IsInt32() ||
    !Nan::Get(event_obj, Nan::New<v8::String>("EvtParam1").ToLocalChecked()).ToLocalChecked()->IsInt32() ||
    !Nan::Get(event_obj, Nan::New<v8::String>("EvtParam2").ToLocalChecked()).ToLocalChecked()->IsInt32() ||
    !Nan::Get(event_obj, Nan::New<v8::String>("EvtParam3").ToLocalChecked()).ToLocalChecked()->IsInt32() ||
    !Nan::Get(event_obj, Nan::New<v8::String>("EvtParam4").ToLocalChecked()).ToLocalChecked()->IsInt32()) {
    return Nan::ThrowTypeError("Wrong argument types");
  }

  Nan::Utf8String *remAddress = new Nan::Utf8String(
    Nan::Get(
        event_obj
      , Nan::New<v8::String>("EvtSender").ToLocalChecked()).ToLocalChecked());

  SrvEvent.EvtTime = static_cast<time_t>(v8::Local<v8::Date>::Cast(Nan::Get(event_obj, Nan::New<v8::String>("EvtTime").ToLocalChecked()).ToLocalChecked())->NumberValue() / 1000);
  SrvEvent.EvtSender = inet_addr(**remAddress);
  SrvEvent.EvtCode = Nan::Get(event_obj, Nan::New<v8::String>("EvtCode").ToLocalChecked()).ToLocalChecked()->Int32Value();
  SrvEvent.EvtRetCode = Nan::Get(event_obj, Nan::New<v8::String>("EvtRetCode").ToLocalChecked()).ToLocalChecked()->Int32Value();
  SrvEvent.EvtParam1 = Nan::Get(event_obj, Nan::New<v8::String>("EvtParam1").ToLocalChecked()).ToLocalChecked()->Int32Value();
  SrvEvent.EvtParam2 = Nan::Get(event_obj, Nan::New<v8::String>("EvtParam2").ToLocalChecked()).ToLocalChecked()->Int32Value();
  SrvEvent.EvtParam3 = Nan::Get(event_obj, Nan::New<v8::String>("EvtParam3").ToLocalChecked()).ToLocalChecked()->Int32Value();
  SrvEvent.EvtParam4 = Nan::Get(event_obj, Nan::New<v8::String>("EvtParam4").ToLocalChecked()).ToLocalChecked()->Int32Value();

  delete remAddress;

  info.GetReturnValue().Set(Nan::New<v8::String>(
    SrvEventText(&SrvEvent).c_str()).ToLocalChecked());
}

}  // namespace node_snap7
