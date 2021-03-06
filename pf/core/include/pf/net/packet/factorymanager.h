/**
 * PLAIN FRAMEWORK ( https://github.com/viticm/plainframework )
 * $Id factorymanager.h
 * @link https://github.com/viticm/plainframework for the canonical source repository
 * @copyright Copyright (c) 2014- viticm( viticm.ti@gmail.com )
 * @license
 * @user viticm<viticm.it@gmail.com>
 * @date 2014/06/22 14:16
 * @uses net packet factory manager
 */
#ifndef PF_NET_PACKET_FACTORYMANAGER_H_
#define PF_NET_PACKET_FACTORYMANAGER_H_

#include "pf/sys/thread.h"
#include "pf/base/singleton.h"
#include "pf/base/hashmap/template.h"
#include "pf/net/config.h"
#include "pf/net/packet/factory.h"

namespace pf_net {

namespace packet {

//目前指针函数有些多，是否考虑更好的方式？ viticm
typedef bool (__stdcall *function_registerfactories)();
typedef bool (__stdcall *function_isvalid_packetid)(uint16_t id);
typedef bool (__stdcall *function_isvalid_dynamic_packetid)(uint16_t id);
typedef bool (__stdcall *function_isencrypt_packetid)(uint16_t id);
typedef uint32_t (__stdcall *function_packetexecute)(connection::Base *, Base *);

class PF_API FactoryManager : public pf_base::Singleton<FactoryManager> {

 public:
   FactoryManager();
   ~FactoryManager();

 public:
   uint32_t *packet_alloccount_;

 public:
   static FactoryManager &getsingleton();
   static FactoryManager *getsingleton_pointer();
 
 public:
   bool init();
   //根据消息类型从内存里分配消息实体数据（允许多线程同时调用，必须用removepacket释放）
   Base *createpacket(uint16_t packetid);
   //根据消息类型取得对应消息的最大尺寸（允许多线程同时调用）
   uint32_t getpacket_maxsize(uint16_t packetid);
   //删除消息实体（允许多线程同时调用，必须和createpacket成对出现）
   void removepacket(Base *packet);
   void lock();
   void unlock();
   bool isvalid_packetid(uint16_t id); //packetid is valid
   bool isvalid_dynamic_packetid(uint16_t id); //dynamic packet id is valid
   void setsize(uint16_t size);
   uint16_t getsize() const;
   void addfactory(Factory *factory);
   bool isencrypt_packetid(uint16_t id); //packetid is encrypt id
   uint32_t packetexecute(connection::Base *connection, Base *packet);

 public: //exports
   void set_function_registerfactories(function_registerfactories function);
   void set_function_isvalid_packetid(function_isvalid_packetid function);
   void set_function_isencrypt_packetid(function_isvalid_packetid function);
   void set_function_isvalid_dynamic_packetid(
       function_isvalid_dynamic_packetid function);
   void set_function_packetexecute(function_packetexecute function);
   bool isinit() const;

 private:
   Factory **factories_;
   pf_base::hashmap::Template<uint16_t, uint16_t> idindexs_;
   pf_base::hashmap::Template<int64_t, Base *> allocpackets_;
   uint16_t size_;
   uint16_t factorycount_;
   pf_sys::ThreadLock lock_;
   bool isinit_; //凡是有内存的初始化都需加上这个标记，以检测再次初始化的情况
   
 private: //exports
   function_registerfactories function_registerfactories_;
   function_isvalid_packetid function_isvalid_packetid_;
   function_isvalid_dynamic_packetid function_isvalid_dynamic_packetid_;
   function_isencrypt_packetid function_isencrypt_packetid_;
   function_packetexecute function_packetexecute_;

};

}; //namespace packet

}; //namespace pap_common_net

PF_API extern pf_net::packet::FactoryManager *g_packetfactory_manager;

#define NET_PACKET_FACTORYMANAGER_POINTER \
  pf_net::packet::FactoryManager::getsingleton_pointer()

#endif //PF_NET_PACKETFACTORY_H_
