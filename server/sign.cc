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
    Reader rd;
    Value Jsinfo, JsinfoDB;

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

        //遍历ID
        leveldb::Iterator *it = IPdb->NewIterator(leveldb::ReadOptions());
        for (it->SeekToFirst(); it->Valid(); it->Next())
        {
            if (ID == it->key().ToString() && rd.parse(it->value().ToString(), JsinfoDB))
            {
                if (rd.parse(it->value().ToString(), JsinfoDB))
                {
                    if (pass == JsinfoDB["pass"].asString())
                    {
                        //登录成功

                        Net::Write(clie_fd, "success", 7);
                        fd_ID[clie_fd] = ID;
                        is_success = true;
                    }
                }
                //(测试用)
                string outpass;
                leveldb::Status status = IPdb->Get(leveldb::ReadOptions(), ID, &outpass);
                check_status(status);
                cout << "[数据库] " << ID << " : " << outpass << endl;
            }
        }

        // ID密码不匹配
        if (!is_success)
        {
            Net::Write(clie_fd, "fail", 4);

            is_success = false;
        }
    }
    return is_success;
}

void Server::sign_up(int clie_fd)
{

    string ID;
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

        //遍历ID
        leveldb::Iterator *it = IPdb->NewIterator(leveldb::ReadOptions());
        for (it->SeekToFirst(); it->Valid(); it->Next())
        {
            if (ID == it->key().ToString())
            {
                Net::Write(clie_fd, "fail", 4);
                id_is_used = true;
                break;
            }
        }

        // ID未被使用
        if (!id_is_used)
        {
            Net::Write(clie_fd, "success", 7);
            leveldb::Status status = IPdb->Put(leveldb::WriteOptions(), ID, info);
            check_status(status);

            //(测试用)
            string outpass;
            status = IPdb->Get(leveldb::ReadOptions(), ID, &outpass);
            check_status(status);
            cout << "[数据库]" << ID << " : " << outpass << endl;
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
                    main_menu(clie_fd);
                }
            }
            bzero(r, sizeof(r));
        }
    }
    return true;
}