#include "local_log.h"

ApplyMsg LocalLog::submit(const Command& command){
    const std::size_t index = entries_.size()+1;
    entries_.push_back({index,command});

    committed_index_ = index;

    CommandResult result = state_machine_.apply(entries_.back().command);
    last_applied_ = index;
    
    return {index,result};
}

std::size_t LocalLog::size() const{
    return entries_.size();
}

std::size_t LocalLog::committedIndex() const {
    return committed_index_;
}

std::size_t LocalLog::lastApplied() const {
    return last_applied_;
}