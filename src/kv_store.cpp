#include "kv_store.h"

void KvStore::put(const std::string& key,const std::string& value){
    data_[key]=value;
}

void KvStore::append(const std::string& key,const std::string& value){
    data_[key]+=value;
}

std::optional<std::string>KvStore::get(const std::string& key)const{
    const auto it = data_.find(key);
    if(it==data_.end()){
        return std::nullopt;
    }
    return it->second;
}