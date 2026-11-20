#include "kv_state_machine.h"

CommandResult KvStateMachine::apply(const Command& command){
    switch (command.operation){
        case Operation::Put:
            store_.put(command.key,command.value);
            return {ResultStatus::Ok,""};
        
            case Operation::Append:
            store_.append(command.key,command.value);
            return {ResultStatus::Ok,""};

            case Operation::Get:{
                const auto value = store_.get(command.key);
            if(!value){
                return {ResultStatus::NotFound,""};
            }
            return {ResultStatus::Ok,*value};
            }    
    }
    return {ResultStatus::InvalidOperation,""};
}