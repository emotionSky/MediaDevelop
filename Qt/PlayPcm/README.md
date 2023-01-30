给出了几个线程版本的示例使用。

其中：

##### 1. std::thread

> PlayPcm-stdthread

使用标准库的thread，没有任何异常，好用。

##### 2. QThread-QT4

> PlayPcm-QThread-QT4

使用QT4的QThread，全程只开辟了一次线程资源，奇怪的点在于这样使用，除了第一次正常播放外，后续都会有刺耳的声音后才能播放。

##### 3. QThread-QT5

> PlayPcm-QThread-QT5

使用QT5的QThread，全程只开辟了一次线程资源，奇怪的点在于这样使用，都会有刺耳的声音后才能播放。

##### 4. 后续优化方向

对于使用QThread，后续改成使用指针，保证线程资源使用一次开辟一次，看看是否能够解决刺耳的声音。