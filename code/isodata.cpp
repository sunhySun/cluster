#include <bits/stdc++.h>
using namespace std;

class isodata{
private:
    int k;          //初始设定的聚类数
    int K;          //期望得到的聚类数
    int theta_n;    //每个聚类中最少的样本数
    int theta_s;    //每个聚类中最大的标准差
    int theta_c;    //每个聚类中最小的距离
    int L;          //一次合并中最大可以合并的对数
    int l;          //迭代次数
    int max_l;      //最大迭代次数
    int n,m;

    vector<vector<double> > X;              //样本
    vector<vector<double> > centriod;       //聚类中心
    vector<vector<int> > C;
    vector<double> D;                       //聚类类内平均距离
    double D_aver;                          //所有类平均距离
    static double **d_cen;

    double **P,**R;
    double **F,*FP,F_all;
public:
    isodata(int nn,int mm,int kk,double *a)
    {
        K=kk;
        k=1;
        theta_n=1;
        theta_s=0.09;
        theta_c=1.2;
        L=1;
        l=0;
        max_l=20;
        n=nn;m=mm;
        P=NULL;R=NULL;F=NULL;FP=NULL;

        for(int i=0;i<n;i++)
        {
            vector<double> tmp;
            for(int j=0;j<m;j++)
            {
                double t=*(a+i*m+j);
                tmp.push_back(t);
            }
            X.push_back(tmp);
        }

        for(int i=0;i<k;i++)
        {
            vector<int> t;
            C.push_back(t);
        }

        init_centriod();
        show_centriod();

        while(true)
        {
            cout<<"--------------------------------------------------------------------------------------"<<endl;
            cluster_assign();
            show_cluster_assign();
            if(delete_cluster())
                continue;
            
            move_centriod();
            show_centriod();

            get_d_average();
            get_all_d_average();
            show_d();

            if(l>=max_l)
                break;
            if(k<=K/2)
                sperate();
            else if(k>=2*K)
                merge();
            else
            {
                cout<<"l="<<l<<endl;
                if(l%2==0)
                {
                    sperate();
                }
                else
                {
                    merge();
                }
                
            }
            l++;
            //cout<<k<<" "<<centriod.size()<<endl;
            show_centriod();
        }

    }

     void init_centriod()
    {
        vector<int> pre;
        //考虑到一般样本数量较多，这里利用vector记录选择的聚类中心的下标
        for(int i=0;i<k;i++)
        {
            srand((int)time(0));
            int t=rand()%n;
            bool flag=false;
            for(int j=0;j<pre.size();j++)
            {
                if(pre[j]==t)
                {
                    flag=true;
                    break;
                }
            }
            if(!flag)
            {
                centriod.push_back(X[t]);
                pre.push_back(t);
            }
            else
            {
                i--;
            }
        }
    }

    void show_centriod()
    {
        cout<<"cluster centriod is :"<<endl;
        for(int i=0;i<k;i++)
        {
            cout<<"(";
            for(int j=0;j<centriod[i].size();j++)
            {
                if(j!=0)
                {
                    cout<<", ";
                }
                cout<<centriod[i][j];
            }
            cout<<")"<<endl;
        }
    }

    void cluster_assign()
    {
        C.resize(k);
        for(int i=0;i<k;i++)
            C[i].clear();
        for(int i=0;i<n;i++)
        {
            int index=-1;
            int mi=-1;
            for(int j=0;j<k;j++)
            {
                double d=get_distance(X[i],centriod[j]);
                if(j==0)
                {
                    mi=d;index=j;
                }
                else
                {
                    if(d<mi)
                    {
                        mi=d;index=j;
                    }
                }
            }
            C[index].push_back(i);
        }
        
    }

    bool delete_cluster()
    {
        int cnt=0;
        for(int i=0;i<k;i++)
        {
            if(C[i].size()<theta_n)
            {
                vector<double> tmp=centriod[i];
                centriod[i]=centriod[k-1];
                centriod[k-1]=tmp;

                vector<int> cc=C[i];
                C[i]=C[k-1];
                C[k-1]=cc;
                k--;i--;
                cnt++;
            }
        }
        bool flag;
        if(cnt>0)
            flag=true;
        else
            flag=false;
        
        while(cnt)
        {
            centriod.pop_back();
            C.pop_back();
            cnt--;
        }
        return flag;
    }

    void show_cluster_assign()
    {
        cout<<"cluster assign:"<<endl;
        for(int i=0;i<k;i++)
        {
            cout<<i<<" : (";
            for(int j=0;j<C[i].size();j++)
            {
                if(j!=0)
                    cout<<", ";
                cout<<C[i][j];
            }
            cout<<")"<<endl;

        }
    }

    bool move_centriod()
    {
        double *val=new double[m];
        double *average=new double[m];
        bool flag=false;
        for(int i=0;i<m;i++)
            average[i]=0;
        for(int i=0;i<k;i++)
        {
            for(int j=0;j<m;j++)
                val[j]=0;
            for(int j=0;j<C[i].size();j++)
            {
                for(int t=0;t<m;t++)
                    val[t]+=X[C[i][j]][t];
            }
            for(int j=0;j<m;j++)
            {
                if(C[i].size()!=0)
                {
                    double t=val[j]/C[i].size();
                    if(centriod[i][j]!=t)
                    {
                        flag=true;
                        centriod[i][j]=t;
                    }
                }
                else
                    centriod[i][j]=0;
                average[j]+=centriod[i][j];
            }   
        }
        for(int i=0;i<k;i++)
        {
            if(C[i].size()>0)
                continue;
            for(int j=0;j<m;j++)
                centriod[i][j]=average[j]/k;
            flag=true;
        }
        return flag;
    }

    double get_distance(vector<double> p,vector<double> q)
    //求样本之间的距离，此处采用欧氏距离
    {
        int t=p.size();
        double sum=0;
        for(int i=0;i<t;i++)
            sum+=(p[i]-q[i])*(p[i]-q[i]);
        return pow(sum,0.5);
    }

    void get_d_average()
    //计算类内平均距离和所有类的平均距离
    {
        for(int i=0;i<k;i++)
        {
            double sum=0;
            for(int j=0;j<C[i].size();j++)
            {
                sum+=get_distance(X[C[i][j]],centriod[i]);
            }
            int t=C[i].size();
            if(D.size()<=i)
                D.push_back(sum/t);
            else
            {
                D[i]=sum/t;
            }
            
        }
    }

    void get_all_d_average()
    {
        D_aver=0;
        for(int i=0;i<k;i++)
        {
            D_aver+=D[i];
        }
        D_aver=D_aver/k;
    }

    void show_d()
    {
        cout<<"The average distance of each cluster:"<<endl;
        for(int i=0;i<k;i++)
        {
            cout<<i<<": "<<D[i]<<endl;
        }

        cout<<"The average distance of all:"<<D_aver<<endl;
    }

    void sperate()
    {
        double alpha=0.3;
        cout<<"sperate...................."<<endl;
        int pre_k=k;
        for(int i=0;i<pre_k;i++)
        {
            double *std_dev=new double[m];
            double ma=-1;
            int index=0;
            for(int j=0;j<m;j++)
            {
                double sum=0;
                for(int t=0;t<C[i].size();t++)
                {
                    double tmp=X[C[i][t]][j]-centriod[i][j];
                    sum+=tmp*tmp;
                }
                std_dev[j]=pow(sum/C[i].size(),0.5);
                if(std_dev[j]>ma)
                {
                    ma=std_dev[j];
                    index=j;
                }
            }
            show_std_dev(std_dev,ma,index,i);
            if((ma>theta_s)&&((D[i]>D_aver&&C[i].size()>2*(theta_n+1))||(k<=K/2)))
            {
                cout<<"1"<<endl;
                k++;
                vector<double> v;
                for(int j=0;j<m;j++)
                {
                    v.push_back(centriod[i][j]-alpha*std_dev[j]);
                    centriod[i][j]=centriod[i][j]+alpha*std_dev[j];
                }
                centriod.push_back(v);
            }
        }
        cout<<"seperate is done."<<endl;
    }

    void show_std_dev(double *std_dev,double ma,int index,int j)
    {
        cout<<"The std_dev of "<<j<<" is:"<<endl;
        cout<<"( ";
        for(int i=0;i<m;i++)
        {
            if(i!=0)
                cout<<", ";
            cout<<std_dev[i];
        }
        cout<<")"<<endl;
        cout<<"The max is std_dev[ "<<index<<" ] = "<<ma<<endl;
    }

    static bool cmp(const pair<int,int> &p,const pair<int,int> &q)
    {
        return d_cen[p.first][p.second]<d_cen[q.first][q.second];
    }

    void show_d_cen(vector<pair<int,int> > &vec)
    {
        for(int i=0;i<vec.size();i++)
        {
            int a1=vec[i].first;
            int a2=vec[i].second;
            cout<<"( "<<a1<<" , "<<a2<<" , "<<d_cen[a1][a2]<<" ) ";
        }
        cout<<endl;
    }

    void merge()
    {
        cout<<"merge........................................."<<endl;
        d_cen=new double* [k];
        vector<pair<int,int> > vec;
        for(int i=0;i<k;i++)
            d_cen[i]=new double[k];
        for(int i=0;i<k;i++)
            for(int j=i+1;j<k;j++)
            {
                d_cen[i][j]=get_distance(centriod[i],centriod[j]);
                vec.push_back(make_pair(i,j));
            }
        
        sort(vec.begin(),vec.end(),cmp);

        show_d_cen(vec);

        int *vis=new int[k];
        int cnt=0;
        for(int i=0;i<k;i++)
            vis[i]=0;
        for(int i=0;i<vec.size();i++)
        {
            double t=d_cen[vec[i].first][vec[i].second];
            int index1=vec[i].first,index2=vec[i].second;
            if(cnt>=L)
                break;
            if(t>theta_c)
                break;
            if(vis[index1]!=0||vis[index2]!=0)
                continue;
            
            int n1=C[index1].size(),n2=C[index2].size();
            for(int j=0;j<m;j++)
            {
                centriod[index1][j]=(centriod[index1][j]*n1+centriod[index2][j]*n2)/(n1+n2);
            }
            vis[index1]=1;vis[index2]=2;
            cnt++;
        }
        for(int i=0;i<k;i++)
        {
            if(vis[i]==2)
            {
                swap(C[i],C[k-1]);
                swap(centriod[i],centriod[k-1]);
                swap(vis[i],vis[k-1]);
                C.pop_back();
                centriod.pop_back();
                k--;
            }
        }
    }

    double get_accuracy(int *ans)
    {
        cluster_assign();
        show_cluster_assign();

        int **intersection;
        intersection=new int* [K];
        P=new double *[K];
        R=new double *[K];
        F=new double *[K];
        int *cnt=new int[K];
        for(int i=0;i<K;i++)
        {
            intersection[i]=new int [k];
            for(int j=0;j<k;j++)
                intersection[i][j]=0;
            P[i]=new double [k];
            R[i]=new double [k];
            F[i]=new double [k];
            cnt[i]=0;
        }
        FP=new double[K];

        for(int i=0;i<n;i++)
        {
            cnt[ans[i]]++;
            for(int j=0;j<k;j++)
            {
                for(int p=0;p<C[j].size();p++)
                {
                    if(C[j][p]==i)
                    {
                        intersection[ans[i]][j]++;
                    }
                }
            }
        }
            
        int index=-1;int sum=0;
        for(int i=0;i<K;i++)
        {
            FP[i]=0;index=-1;
            for(int j=0;j<k;j++)
            {
                if(intersection[i][j]==0)
                {
                    P[i][j]=R[i][j]=F[i][j]=0;
                }
                else
                {
                    int tmp=C[j].size();
                    P[i][j]=1.0*intersection[i][j]/tmp;
                    R[i][j]=1.0*intersection[i][j]/cnt[i];
                    F[i][j]=(2*P[i][j]*R[i][j])/(P[i][j]+R[i][j]);
                }                
                if(F[i][j]>FP[i])
                {
                    FP[i]=F[i][j];
                    index=j;
                }
                //cout<<i<<" "<<j<<" "<<P[i][j]<<" "<<R[i][j]<<" "<<F[i][j]<<endl;
            }
            F_all+=cnt[index]*FP[i];
            sum+=cnt[index];
        }
        F_all=F_all/sum;
        return F_all;
    }
};

double** isodata::d_cen=NULL;

double get_double (char *s,int i)
{
    double n=0;
    while(s[i]>='0'&&s[i]<='9')
    {
        n=n*10+s[i]-'0';
        i++;
    }
    if(s[i]=='.')
        i++;
    double x=1;
    while(s[i]>='0'&&s[i]<='9')
    {
        x*=0.1;
        n+=x*(s[i]-'0');
        i++;
    }
    return n;
}

int get_item(char *s,int index)
{
    char str[3][20]={"Iris-setosa","Iris-versicolor","Iris-virginica"};
    
    for(int i=0;i<3;i++)
    {
        bool flag=true;
        int l=strlen(str[i]);
        int k=index;
        for(int j=0;j<l;j++)
        {
            if(str[i][j]!=s[k])
            {
                flag=false;break;
            }
            k++;
        }
        if(flag)
            return i;
    }
    return 3;
}

void input_iris(double *a,int *ans,int &n,int &m,int &k)
{
    FILE *fp;
    if((fp=fopen("../data/iris/iris.data","r")))
        cout<<"Open File Successfully!"<<endl;
    else
    {
        cout<<"Open File Failed!"<<endl;
        return;
    }
    
    
    char *s=new char[60];
    for(int i=0;i<n;i++)
    {
        fgets(s,50,fp);
        //printf("%s\n",s);
        for(int j=0;j<m;j++)
        {
            *(a+i*m+j)=get_double(s,j*4);
           // cout<<(*(a+i*m+j))<<" ";
        }
        *(ans+i)=get_item(s,m*4);
        //cout<<"||"<<(*(ans+i))<<endl;
    }
}

int main()
{
    double *a;
    int *ans;
    int n,m,k;
    n=150;m=4;k=3;
    a=new double[n*m];
    ans=new int[n];
    input_iris(a,ans,n,m,k);
    isodata K(n,m,k,a);
    cout<<"The accuracy is "<<K.get_accuracy(ans)<<endl;
    system("pause");
    return 0;
}