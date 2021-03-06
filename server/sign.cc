#include "net_wrap.h"
#include "serverClass.h"

/**
 *
 *
 *   登录，注册操作
 *
 *
 */
bool Server::sign_in(int clie_fd)
{
    bool is_success = false;

    string ID, pass;
    string gets;
    Reader rd;
    Value Jsinfo, JsinfoDB;
    Value getv;

    char info[BUFSIZ];

    while (true)
    {
        bzero(info, sizeof(info));
        if ((read(clie_fd, info, sizeof(info))) > 0)
        {
            cout << "[客户端] " << clie_fd << " : " << info << endl;
            break;
        }
    }

    if (rd.parse(info, Jsinfo))
    {
        ID = Jsinfo["ID"].asString();
        pass = Jsinfo["pass"].asString();

        leveldb::Status s = IPdb->Get(leveldb::ReadOptions(), ID, &gets);
        if (s.ok())
        {
            rd.parse(gets, getv);
            if (pass == getv["pass"].asString())
            {

                Net::Write(clie_fd, "success", 7);
                fd_ID[clie_fd] = ID;
                is_success = true;
            }
            else
            {
                Net::Write(clie_fd, "fail", 4);
            }
        }
        else
        {
            Net::Write(clie_fd, "fail", 4);
            is_success = false;
        }
    }
    return is_success;
}

void Server::sign_up(int clie_fd)
{
    string path;
    string ID, get;
    Reader rd;
    Value Jsinfo;

    char info[BUFSIZ];
    bool id_is_used = false;

    while (true)
    {

        if ((read(clie_fd, info, sizeof(info))) > 0)
        {
            cout << " [客户端]" << clie_fd << ":" << info << endl;

            break;
        }
    }

    if (rd.parse(info, Jsinfo))
    {
        ID = Jsinfo["ID"].asString();

        leveldb::Status s = IPdb->Get(leveldb::ReadOptions(), ID, &get);
        if (s.ok()) //该ID不存在为0
        {
            Net::Write(clie_fd, "fail", 4);
            id_is_used = true;
        }

        // ID未被使用
        if (!id_is_used)
        {
            Net::Write(clie_fd, "success", 7);
            leveldb::Status status = IPdb->Put(leveldb::WriteOptions(), ID, info);
            check_status(status);
            path = PATHP + ID + "/";
            mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO);
        }
    }

    return;
}

bool Server::sign_menu(int clie_fd)
{
    cout << "进入sign" << endl;
    char r[BUFSIZ];
    //登录
    while (fd_in[clie_fd] == false)
    {
        if (read(clie_fd, r, sizeof(r)) > 0)
        {
            cout << "sign循环" << endl;

            if (strcmp(r, EXIT) == 0)
            {
                cout << "----" << clie_fd << "已退出----" << endl;
                Net::Close(clie_fd);
                break;
            }
            else if (strcmp(r, SIGN_UP) == 0)
            {
                Server::sign_up(clie_fd);
            }
            else if (strcmp(r, SIGN_IN) == 0)
            {
                fd_in[clie_fd] = Server::sign_in(clie_fd);
                if (fd_in[clie_fd])
                {
                    // thread remind(Server::remind, clie_fd);
                    // remind.detach();
                    main_menu(clie_fd);
                }
            }
            bzero(r, sizeof(r));
        }
    }
    return true;
}

// void Server::remind(int clie_fd)
// {
//     Value getv, member;
//     string gets;
//     string send;
//     Reader rd;
//     FastWriter w;

//     int i = 0;

//     while (true)
//     {
//         sleep(0.5);
//         leveldb::Status s = NMdb->Get(leveldb::ReadOptions(), fd_ID[clie_fd], &gets);
//         rd.parse(gets, getv);
//         for (; i < (int)getv.size(); i++)
//         {
//             sleep(0.1);
//             member = getv[i];
//             send = w.write(member);
//             cout << "已发送" << send << endl;
//             Net::Write(clie_fd, send.c_str(), send.length());
//         }
//     }
// }