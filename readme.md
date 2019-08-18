# 前提描述
1. 共有5个摄像头，每个的编号为1 2 4 8 16 即 1 10 100 1000 10000
# 信息采集节点流程描述
1. 每个摄像头单独采集数据，当首次检测到人体的时候，每隔0.5S进行广播通告，广播的内容为整数 自己的ID，初始result为0，用0和自己的ID进行或运算，得到新的result跳转到2
2. 发出yes之后进入阻塞态，在端口2019阻塞接收广播报文，总阻塞时间为10s，每次接受到报文之后，就把报文中的ID提取出来，与result进行运算，看看是否等于31，若等于跳转到3，若超时跳转到10
3. 提示用户10s后开始，提示音：“10s后开始”，跳转到
4. 提示音：“现在开始，请从站立动作为开始，做完动作之后，恢复为站立动作”，跳转到
5. 如果检测不到，开始保存图片，保存到第一次可以检测到为止，记录保存了多少张（picturenum）图片，以picturenum.png保存图片
6. 告诉python进程picturenum的值，python进程把(picturenum/2)+ - 0 1 3 5.png 用yolo提取出人的范围，并剪裁，提交给server，多线程提交
7. 把这5张图片发送给服务器
8. 如果此时窗口有waitKey活动，则广播"exit"，跳转到10
9. 若收到"exit"，跳转到10，否则跳转到4
10. 退出程序

# 服务器节点功能描述
1. 开放接口 /BehaviorRecognition/who/<str:start/done_id> 如果有数据要发送，先从这个接口发一下自己的ID，start参数，服务器保存一个log文件，检测摄像头的提交情况，发送完成之后，也要访问一次这个接口，done参数，提交自己的ID，收到done参数要给matlab的server发送一个包，告诉谁的文件好了
2. 开放接口 /BehaviorRecognition/PostPicture 提交图片，命名方式 id-0.png id-1.png ... id-5.png


# 流程描述
1. 开机之后等待接入
2. 如果访问/BehaviorRecognition/who/start_id 接口则log一下时间戳和id
3. 如果访问/BehaviorRecognition/who/done_id 接口则log一下时间戳和id，并且通知matlab id
4. /BehaviorRecognition/PostPicture 提交图片，需要客户端自己命名好

# 资源提取
1. SVM_HOG.xml 
链接: https://pan.baidu.com/s/1skjyqsHjHiX2gAIoDUh8CQ 提取码: qexy 

2. 