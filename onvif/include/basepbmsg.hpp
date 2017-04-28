
#include <string>
#include <iostream>
#include <assert.h>
#include <stdint.h>

using namespace std;

class BasePBMsgHanle
{
    public:
	
#if 0
    void initHandles()
    {
        registerHandle(&BasePBMsgHanle::handleProtoPerson);
        registerHandle(&BasePBMsgHanle::handleProtoTest);
    }
#endif
    
    /*  ����������Ϣ
     *  data Ϊһ�����������ݰ�
     */
    void    handle(const char* data)
    {
        bool ret = false;
        
        const char * current=data;
        
        //�������ϴ����һ�����ݰ��ܳ���
        int packetLength=0;
        
        //�ӵ�һ��λ���ϻ�ȡ�����ݰ��ܳ���
        memcpy(&packetLength, data, sizeof(int32_t));
        
        //ָ�����
        current+=sizeof(int32_t);
        
        //Message���ֵĳ���
        int protoNameLength=0;
        
        //�ӵڶ���λ���ϻ�ȡMessage�����ֵĳ���
        memcpy(&protoNameLength, current, sizeof(int32_t));
        
        //ָ�����
        current+=sizeof(int32_t);
        
        //�ӵ�����λ���ϻ�ȡMessage������
        string name(current,protoNameLength);
        
        //ָ�����
        current+=protoNameLength;
        
        //ȡ��Message���ֽ���
        int messageSize=packetLength-(sizeof(int32_t)+sizeof(int32_t)+protoNameLength);
        
        do{
            
            msg_handle callback = m_callbacks[name];
            
            assert(callback != NULL);
            
            if(callback == NULL)
            {
                std::cout<<"proto "<<name<<" had not register handler"<<std::endl;
                break;
            }
            const ::google::protobuf::Descriptor* descriptor = m_descriptors[name];
            assert(descriptor != NULL);
            if(descriptor == NULL)
            {
                std::cout<<"proto "<<name<<" had no descriptor"<<std::endl;
                break;
            }
            const google::protobuf::Message* prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
            assert(prototype != NULL);
            if(prototype == NULL)
            {
                std::cout<<"proto "<<name<<" had no prototype"<<std::endl;
                break;
            }
            google::protobuf::Message* msg = prototype->New();
            ret = msg->ParseFromArray(current,messageSize);
            if(ret)
            {
                (this->*callback)(msg);
            }
            else
            {
                std::cout<<"proto "<<name<<" parse fail"<<std::endl;
            }
         
        }while(0);
    }
private:
#if 0
    void handleProtoTest(test* test)
    {
        cout <<"test->price()="<< test->price() << endl;
        cout << "test->userid()="<<test->userid() << endl;
        cout << "test->time()="<<test->time() << endl;
    }
    void handleProtoPerson(person* person)
    {
        cout << "person->age()="<<person->age() << endl;
        cout << "person->userid()="<<person->userid() << endl;
        cout << "person->name()="<<person->name() << endl;
    }
#endif
    
private:
    
    typedef void (BasePBMsgHanle::*msg_handle)(::google::protobuf::Message*);
    
	/* all the sub class register to here */
    static map<string, msg_handle>                                 m_callbacks;
    
    static map<string, const ::google::protobuf::Descriptor*>      m_descriptors;

    template<typename MSGTYPE>
    void registerHandle(void (BasePBMsgHanle::*callback)(MSGTYPE*))
    {
        const ::google::protobuf::Descriptor*des =MSGTYPE::descriptor();
        assert(des != NULL);
        if(des != NULL)
        {
            m_callbacks[des->full_name()] = (msg_handle)callback;
            m_descriptors[des->full_name()] = des;
        }
    }

 
};

class BasePBMsgBuilder
{
    public:
    /*  ����proto msg��ָ��������
     *  int32_t   packetLength ���ݰ��ܳ���
     *  int32_t   messageNameLength ��Ϣ������
     *  char[]    messageName ��Ϣ��
     *  char[]    Message ��Ϣ
     *  char*     buffer ������
     *  int       maxLength ��Ϣ����󳤶�
     */
    template<typename MSGTYPE>
    static int sendMessageToBuffer(MSGTYPE& msg, char* buffer, int maxLength){
        
        char * current=buffer;
        
        //Message���ֽ���
        int messageSize=msg.ByteSize();
        
        //Message������
        string messageName=MSGTYPE::descriptor()->full_name();
        
        //Message���ֵĳ���
        size_t messageNameLength=messageName.size();
        
        //��Ϣ��� messageSize+messageNameLength+messageName+Message
        size_t packetLength=sizeof(int32_t)+sizeof(int32_t)+messageNameLength+messageSize;
        
        if (packetLength>maxLength) {
            return -1;
        }
        
        //�����ݰ��ܳ��ȷ��ڵ�һ��λ��
        memcpy(current, &packetLength, sizeof(int32_t));
        
        //ָ�����
        current+=sizeof(int32_t);
        
        //��Message���Ƴ��ȷ��ڵڶ���λ��
        memcpy(current, &messageNameLength, sizeof(int32_t));
        
        //ָ�����
        current+=sizeof(int32_t);
        
        //��Э�����Ʒ��ڵ�����λ����
        strcpy(current,messageName.c_str());
        
        //ָ�����
        current+=messageNameLength;
        
        //��Message���ڵ��ĸ�λ����
        msg.SerializeToArray(current,messageSize);
        
        return (int)packetLength;
        
    }
};

#if 0
int main()
{
    
    BasePBMsgHanle msghandle;
    
    msghandle.initHandles();
    
    test t;
    
    t.set_price(100.0);
    
    t.set_userid(110);
    
    t.set_time(123);
    
    person person;
    
    person.set_age(18);
    
    person.set_userid(200508);
    
    person.set_name("irons");
    
    char tmp[10*1024];
    
    BasePBMsgBuilder::sendMessageToBuffer(t, tmp, sizeof(tmp));
    
    msghandle.handle(tmp);
    
    BasePBMsgBuilder::sendMessageToBuffer(person, tmp, sizeof(tmp));
    
    msghandle.handle(tmp);
    
    cin.get();
    
    return 0;
}

#endif

    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    

