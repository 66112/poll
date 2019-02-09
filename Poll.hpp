/*
 *1.poll的接口更加方便
 *2.文件描述符没有上限,但太多了也会影响性能
 *缺点:
 *1.poll返回后,需要用轮询的方式从struct pollfd数组中获得就绪的文件描述符
 *2.随着监视的描述符数量增长,其效率会直线下降
 *3.每次调用poll都需要把大量的pollfd结构调用从用户态拷贝到内核态
 * */
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#define POLL_SIZE 1024
class Pollor
{
public:
    Pollor():_poll_pfd(new pollfd[POLL_SIZE]),_tru_num(0)
    {}
    void Init_Poll()
    {
        for(int i = 0; i < POLL_SIZE; i++){
            _poll_pfd[i].fd = -1;
            _poll_pfd[i].events = 0;
            _poll_pfd[i].revents = 0;
        }
    }
    ~Pollor()
    {
        delete[] _poll_pfd;
    }
    void Add_fd(pollfd pol_fd_)
    {
        for(int i = 0; i < POLL_SIZE; i++){
            if(_poll_pfd[i].fd == -1){
                _poll_pfd[i] = pol_fd_;
                _tru_num++;
                break;
            }
        }
    }
    void Insert_cli_msg(int fd,sockaddr_in& client)
    {
        _cli_msg.insert(std::pair<int,sockaddr_in>(fd,client));
    }
    void Del_fd(int fd)
    {
        for(int i = 0; i < POLL_SIZE; i++){
            if(_poll_pfd[i].fd == fd){
                _poll_pfd[i].fd = -1;
                _poll_pfd[i].events = 0;
                _poll_pfd[i].revents = 0;
                _tru_num--;
                break;
            }
        }
        std::map<int,sockaddr_in>::iterator it;
        it = _cli_msg.find(fd);
        _cli_msg.erase(it); 
    }
    void Find_cli(int fd,sockaddr_in& cli_)
    {
        std::map<int,sockaddr_in>::iterator it;
        it = _cli_msg.find(fd);
        cli_ = it -> second;
    }
    int Wait_Poll()
    {
        int ret;
        switch (ret = poll(_poll_pfd,POLL_SIZE,1000)){
            case -1:
                std::cerr << "poll error!" << std::endl;
                break;
            case 0:
                std::cerr << "timeout!!!" << std::endl;
                break;
            default:
                for(int i = 0; i < POLL_SIZE; i++){
                    if(!Is_OK_rd(_poll_pfd[i])) continue;
                    ret =  _poll_pfd[i].fd;                        
                }
                return ret;
        }
        return ret - 1;
    }
private:
    bool Is_OK_rd(pollfd& pol_fd_)
    {
        return pol_fd_.revents & POLLIN;
    }
private:
    std::map<int,sockaddr_in> _cli_msg;
    struct pollfd* _poll_pfd;  //指针指向结构体数组
    int _tru_num;              //实际由多少个可用元素
};
