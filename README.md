# cluster
## 概述
在学习的过程中，结合kmeans、kmeans++和isodata的算法，用c++实现了以上三种算法，并用iris作为数据集进行测试，其中代码均存放在code文件夹下，数据均存放在data数据夹下

## 运行
* kmeans.cpp  
实现kmeans算法，由于通过随机数初始化聚类中心，所以结果准确率会波动  
* kmeans++.cpp  
在kmeans基础上，实现kmeans++算法  
* isodata.cpp 
实现isodata算法  
  >相关参数为：  
  初始聚类中心个数：k=1;  
  每个类别最少样本数：theta_n=1;  
  每个类别最大标准差：theta_s=0.09;  
  两个聚类中心最小距离：theta_c=1.2;  
  一次合并中，可合并的最多对数：L=1;  
  最大迭代次数：max_l=20;  

* 直接运行exe文件就可以查看结果
