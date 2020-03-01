# boost_web

*boost_web is a simple web service (http(s)/websocket(s)) wrapper with C++11 while base on boost.beast*



## build

1. download boost (version 1.70.0 at least) from www.boost.org and build it first
2. download boost_web, go to folder *sln*, change the boost include and library path of file *boost_web.vcxproj* / *Makefile_a* / *Makefile_so*
3. for **Windows**, open *boost_web.sln* with **virtual studio 2017**, build it
4. for **Linux** / **OS X**, run command: `make -f Makefile_a`  for static library, or `make -f Makefile_so` for dynamic library
5. the files *boost_web.h*, *boost_web.lib*, *boost_web.dll* are what we need



## usage

the files *test/test_service.h* and *echo/echo_service.h* have show you how to use it, and a quick explanation here

1. the first thing is include the head file and library

   ```c++
   #include "boost_web.h"
   ```

2. then you need to define a class to inherit from **WebServiceBase**，and implement the pure virtual functions of the base class

   ```c++
   class TestService : public BoostWeb::WebServiceBase
   {
       ...
   public: /* http(s) */
       virtual bool target_is_path(const std::string & target) override;
       virtual bool handle_request(const BoostWeb::HttpConnectionBase & connection, const BoostWeb::HttpRequestBase & request, BoostWeb::HttpResponseBase & response) override;
       virtual void on_error(const char * protocol, const char * what, int error, const char * message) override;
   
   public: /* websocket(s) */
       virtual bool on_connect(BoostWeb::WebsocketConnectionSharedPtr connection, std::size_t identity) override;
       virtual bool on_accept(BoostWeb::WebsocketConnectionSharedPtr connection, unsigned short listener_port) override;
       virtual bool on_recv(BoostWeb::WebsocketConnectionSharedPtr connection) override;
       virtual bool on_send(BoostWeb::WebsocketConnectionSharedPtr connection) override;
       virtual void on_error(BoostWeb::WebsocketConnectionSharedPtr connection, const char * protocol, const char * what, int error, const char * message) override;
       virtual void on_close(BoostWeb::WebsocketConnectionSharedPtr connection) override;
       ...
   }
   ```
   
3. we define the **WebManager** instance to establish connections and send / receive data 

   ```c++
   class TestService : public BoostWeb::WebServiceBase
   {
       ...
   private:
       BoostWeb::WebManager                                        m_web_manager;
   }
   ```
   
4. we define *init()* and *exit()* for start and stop the test service

   ```c++
   class TestService : public BoostWeb::WebServiceBase
   {
       ...
   public:
       bool init();
       void exit();
       ...
   }
   ```

   - if the **TestService**  is a **server** (the sample service support *http / https / ws / wss* on a same tcp port, others will connect to its port)

     ```c++
     bool TestService::init()
     {
         const char * crt_file = "f:/codes/res/test.crt";
         const char * key_file = "f:/codes/res/test.key";
         BoostWeb::ServiceNode service_node;
         service_node.host = "0.0.0.0";
         service_node.port = 12345;
         service_node.root = "f:/codes/web/html/";
         service_node.timeout = 15; /* seconds */
         service_node.body_limit = 0;
         service_node.protocol = BoostWeb::support_protocol_t::protocol_all;
         return (m_web_manager.init(this, &service_node, 1, true, crt_file, key_file, 1));
     }
     ```
     
   - if the **TestService** is a pure **client** (only connect to other's ports, the sample service as a *ws / wss* client, **note** that **not support** *http / https* client)
     
     ```c++
     bool TestService::init()
     {
         return (m_web_manager.init(this));
     }
     ```
     
   - easy to connect the other server
     
     ```
     {
         const char * ws_host = "172.16.4.33";
         const char * ws_root = "/";
     
         unsigned short ws_port_1 = 9001;
         std::size_t ws_identity_1 = 11111; /* when you need to identify the connection */
         if (!m_ws_manager.create_ws_client(ws_host, ws_port_1, ws_root, ws_identity_1))
         {
             return (false);
         }
     
         unsigned short ws_port_2 = 9002;
         std::size_t ws_identity_2 = 22222; /* when you need to identify the connection */
         if (!m_ws_manager.create_ws_client(ws_host, ws_port_2, ws_root, ws_identity_2))
         {
             return (false);
         }
     
         const char * wss_host = "192.168.1.113";
         const char * wss_root = "/";
     
         unsigned short wss_port_1 = 9011;
         std::size_t wss_identity_1 = 33333; /* when you need to identify the connection */
         if (!m_wss_manager.create_wss_client(wss_host, wss_port_1, wss_root, wss_identity_1))
         {
             return (false);
         }
     
         unsigned short wss_port_2 = 9012;
         std::size_t wss_identity_2 = 44444; /* when you need to identify the connection */
         if (!m_wss_manager.create_wss_client(wss_host, wss_port_2, wss_root, wss_identity_2))
         {
             return (false);
         }
     
         return (true);
     }
     ```
   
   - easy to stop **TestService**
     
     ```c++
     void TestService::exit()
     {
         m_web_manager.exit();
     }
     ```
   
5. when a *http / https* request comes , **target_is_path**(*target*) will callback first, you should judge *target* is  *normal url target* or *file relative path* (of service-node-root on **WebManager::init()** )  in it, if *target* is *normal url target*, **handle_request**(*connection*, *request*, *response*) will be callback late，but if *target* is *file relative path*, the file context will be send automatically

   ```c++
   bool TestService::target_is_path(const std::string & target)
   {
       return ("/api/" != target.substr(0, 5));
   }
   ```
   
6. when a *http / https* request comes , and *target* is *normal url target*, **handle_request**(*connection*, *request*, *response*) will be callback, you should *parse the request* and *build the response*, then the response will been send to *http / https* client automatically, callback return value will be ignore

   ```c++
   bool TestService::handle_request(const BoostWeb::HttpConnectionBase & connection, const BoostWeb::HttpRequestBase & request, BoostWeb::HttpResponseBase & response)
   {
       std::string host_ip;
       unsigned short host_port = 0;
       connection.get_host_address(host_ip, host_port);
       std::string peer_ip;
       unsigned short peer_port = 0;
       connection.get_peer_address(peer_ip, peer_port);
       printf("http(s) session: [%s:%u] -> [%s:%u]\n", host_ip.c_str(), host_port, peer_ip.c_str(), peer_port);
       response.set_result(BoostWeb::http_result_t::http_ok);
       printf("method: %s\n", request.get_method_string().c_str());
       printf("target: %s\n", request.get_target().c_str());
       printf("cookie: %s\n", request.get_field_cookie().c_str());
       printf("body: {\n%s\n}\n", request.get_body().c_str());
       if (BoostWeb::http_method_t::http_post == request.get_method())
       {
           const std::string content_type(request.get_field_content_type());
           if (std::string::npos != content_type.find("multipart/form-data"))
           {
               printf("it is a post file request\n");
               const std::string boundary_mark("boundary=");
               std::string::size_type boundary_pos_beg = content_type.find(boundary_mark);
               if (std::string::npos != boundary_pos_beg)
               {
                   boundary_pos_beg += boundary_mark.size();
                   std::string::size_type boundary_pos_end = content_type.find(';', boundary_pos_beg);
                   const std::string boundary(content_type.substr(boundary_pos_beg, std::min<std::string::size_type>(boundary_pos_end, content_type.size() - boundary_pos_beg)));
                   printf("boundary is (%s)\n", boundary.c_str());
                   parse_file_body(boundary, request.get_body()); /* see test/test.h */
               }
               else
               {
                   printf("boundary is not found\n");
               }
           }
       }
       response.set_body("recv request (" + request.get_method_string() + ": " + request.get_target() + ", {" + request.get_body() + "})");
       return (true);
   }
   ```

7. when peer is a *http / https* client, **on_error**(*protocol*, *what*, *error*, *message*) will be callback

   ```c++
   void TestService::on_error(const char * protocol, const char * what, int error, const char * message)
   {
       printf("%s::%s error (%u) {%s}\n", protocol, what, error, message);
   }
   ```

8. when peer is a *ws / wss* client, **on_error**(*connection*, *protocol*, *what*, *error*, *message*) will be callback, **note** that *connection* maybe is nullptr

   ```c++
   void TestService::on_error(BoostWeb::WebsocketConnectionSharedPtr connection, const char * protocol, const char * what, int error, const char * message)
   {
       if (!!connection)
       {
           std::string host_ip;
           unsigned short host_port = 0;
           connection.get_host_address(host_ip, host_port);
           std::string peer_ip;
           unsigned short peer_port = 0;
           connection.get_peer_address(peer_ip, peer_port);
           printf("connection: (%s:%u - %s:%u) error\n", host_ip.c_str(), host_port, peer_ip.c_str(), peer_port);
       }
       printf("%s::%s error (%u) {%s}\n", protocol, what, error, message);
   }
   ```

9. when **client** active-connect complete , **on_connect**(*connection*, *identity*) will callback, *identity* identify which connect-operation, if *connection* is *nullptr* means connect failed, **note** that the **return value is very important**, if return false means that the current connection is abandoned, which is equivalent to calling *connection->close()* implicitly

   ```c++
   bool TestService::on_connect(BoostWeb::WebsocketConnectionSharedPtr connection, std::size_t identity)
   {
       if (!connection)
       {
           /* maybe we want retry */
           if (11111 == identity) /* the first ws connect operate failed */
           {
               m_web_manager.create_ws_client("172.16.4.33", 9001, "/", 11111); 
           }
           else if (22222 == identity) /* the second ws connect operate failed */
           {
               m_web_manager.create_ws_client("172.16.4.33", 9002, "/", 22222); 
           }
           else if (33333 == identity) /* the first wss connect operate failed */
           {
               m_web_manager.create_wss_client("192.168.1.113", 9011, "/", 33333); 
           }
           else if (44444 == identity) /* the second wss connect operate failed */
           {
               m_web_manager.create_wss_client("192.168.1.113", 9012, "/", 44444); 
           }
           else
           {
               assert(false); /* unknown connection, never come here */
           }
           return (false);
       }
       else
       {
           /* we can store identity as a user data, if we want */
           connection->set_user_data(reinterpret_cast<void *>(identity));
            
           /* also we can store connection with a member variable */
           if (11111 == identity) /* the first ws connect operate success */
           {
               m_ws_connection_1 = connection;
           }
           else if (22222 == identity) /* the second ws connect operate success */
           {
               m_ws_connection_2 = connection;
           }
           else if (33333 == identity) /* the first wss connect operate success */
           {
               m_wss_connection_1 = connection;
           }
           else if (44444 == identity) /* the second wss connect operate success */
           {
               m_wss_connection_2 = connection;
           }
           else
           {
               assert(false); /* unknown connection, never come here */
           }
           return (true);
       }
   }
   ```

10. when **TestService** is a **server**, and others connect to its bind-ports, **on_accept**(*connection*, *listener_port*) will callback, *listener_port* identify which port connect from, the *connection* never be *nullptr* here, **note** that the **return value is very important**, if return false means that the current connection is abandoned, which is equivalent to calling *connection->close()* implicitly

    ```c++
    bool TestService::on_accept(BoostWeb::WebsocketConnectionSharedPtr connection, unsigned short listener_port)
    {
        assert(!!connection);
        /* maybe we need increase the number of connections on each listener port */
        if (10001 == listener_port)
        {
            ++m_port_count_1; /* we do not know it is ws or wss connection */
        }
        else if (10002 == listener_port)
        {
            ++m_port_count_2; /* we do not care it is ws or wss connection */
        }
        else
        {
            assert(false); /* unknown listener port, never come here */
        }
        return (true);
    }
    ```

11. when connection close, **on_close**(*connection*) will callback, it is the last callback of connection, the *connection* never be *nullptr* here

    - if **TestService** is **client** maybe we do like this

      ```c++
      void TestService::on_close(BoostWeb::WebsocketConnectionSharedPtr connection)
      {
          assert(!!connection);
          /* we have stored identity as a user data before, now can load it */
          std::size_t identity = reinterpret_cast(connection->get_user_data());
          /* maybe we want to reconnect */
          if (11111 == identity) /* it is the first ws connection close */
          {
              m_ws_connection_1.reset();
              m_web_manager.create_ws_client("172.16.4.33", 9001, "/", 11111); 
          }
          else if (22222 == identity) /* it is the second ws connection close */
          {
              m_ws_connection_2.reset();
              m_web_manager.create_ws_client("172.16.4.33", 9002, "/", 22222); 
          }
          else if (33333 == identity) /* it is the first wss connection close */
          {
              m_wss_connection_1.reset();
              m_web_manager.create_wss_client("192.168.1.113", 9011, "/", 33333); 
          }
          else if (44444 == identity) /* it is the second wss connection close */
          {
              m_wss_connection_2.reset();
              m_web_manager.create_wss_client("192.168.1.113", 9012, "/", 44444); 
          }
          else
          {
              assert(false); /* unknown connection, never come here */
          }
      }
      ```

      or do like another way

      ```c++
      void TestService::on_close(BoostWeb::WebsocketConnectionSharedPtr connection)
      {
          assert(!!connection);
          /* maybe we want to reconnect */
          if (connection == m_ws_connection_1) /* it is the first ws connection close */
          {
              m_ws_connection_1.reset();
              m_web_manager.create_ws_client("172.16.4.33", 9001, "/", 11111); 
          }
          else if (connection == m_ws_connection_2) /* it is the second ws connection close */
          {
              m_ws_connection_2.reset();
              m_web_manager.create_ws_client("172.16.4.33", 9002, "/", 22222); 
          }
          else if (connection == m_wss_connection_1) /* it is the first wss connection close */
          {
              m_wss_connection_1.reset();
              m_web_manager.create_wss_client("192.168.1.113", 9011, "/", 33333); 
          }
          else if (connection == m_wss_connection_2) /* it is the second wss connection close */
          {
              m_wss_connection_2.reset();
              m_web_manager.create_wss_client("192.168.1.113", 9012, "/", 44444); 
          }
          else
          {
              assert(false); /* unknown connection, never come here */
          }
      }
      ```

    - if **TestService** is **server** maybe we do like this

      ```c++
      void TestService::on_close(BoostWeb::WebsocketConnectionSharedPtr connection)
      {
          assert(!!connection);
          /* maybe we need decrease the number of connections on each listener port */
          std::string host_ip;
          unsigned short host_port = 0;
          connection->get_host_address(host_ip, host_port);
          if (10001 == host_port)
          {
              --m_port_count_1;
          }
          else if (10002 == host_port)
          {
              --m_port_count_2;
          }
          else
          {
              assert(false); /* unknown listener port, never come here */
          }
      }
      ```
      

12. when received any new data, **on_recv**(*connection*) will callback, the *connection* never be *nullptr* here, **note** that the **return value is very important**, if return false means that the current connection is abandoned, which is equivalent to calling *connection->close()* implicitly

    ```c++
    bool TestService::on_recv(BoostWeb::WebsocketConnectionSharedPtr connection)
    {
        /*
         * use ‘connection->recv_buffer_has_data()’ to check whether has more frame data
         * use ‘connection->recv_buffer_data_is_text()’ to check the first frame data is text or binary
         * use ‘connection->recv_buffer_data()’ to get the first frame data point
         * use ‘connection->recv_buffer_size()’ to get the first frame data size
         * use ‘connection->recv_buffer_drop()’ to drop first frame data
         * use ‘connection->send_buffer_fill(text, data, len)’ to send frame data
         */
        assert(!!connection);
        /* maybe we want just send it back here */
        while (connection->recv_buffer_has_data())
        {
            bool text = connection->recv_buffer_data_is_text();
            const char * data = reinterpret_cast<const char *>(connection->recv_buffer_data());
            std::size_t size = connection->recv_buffer_size();
            if (!connection->send_buffer_fill(text, data, size))
            {
                return (false); // send failed, return false for notify to close it
            }
            if (!connection->recv_buffer_drop())
            {
                return (false); // something wrong, return false for notify to close it
            }
        }
        return (true);
    }
    ```

13. when all data has sent complete, **on_send**(*connection*) will callback, the *connection* never be *nullptr* here, **note** that the **return value is very important**, if return false means that the current connection is abandoned, which is equivalent to calling *connection->close()* implicitly, so mostly we just return true 

    ```c++
    bool TestService::on_send(BoostWeb::WebsocketConnectionSharedPtr connection)
    {
        assert(!!connection);
        return (true);
    }
    ```

14. **note** that each **callback** for each connection is **blocked**, so don't do anything too time-consuming within the callback

15. **note** that each **callback** for each connection is **mutually exclusive**, so  we need not any mutex to protect it, but if we save the *connection* as a member variable and use it in non-callback functions (meaning other threads), pay attention to the usage of smart pointer member variable

    ```c++
    {
        /* 1, bad way to use smart pointer member variable */
        if (!!m_ws_connection_1)
        {
            /*
             * maybe on_close(connection) called at the same time
             * and m_ws_connection_1.reset() been called there
             * the next operate will get a bomb
             */
            m_ws_connection_1->send_buffer_fill("hello", 5);
        }
        
        /* 2, right way to use smart pointer member variable */
        BoostWeb::WebsocketConnectionSharedPtr connection = m_ws_connection_1;
        if (!!connection)
        {
            connection->send_buffer_fill("hello", 5);
        } 
    }
    ```

    

