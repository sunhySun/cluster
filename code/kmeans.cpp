#include <bits/stdc++.h>
using namespace std;

class kmeans{
public:
    vector<vector<double> > X;
    vector<vector<double> > centriod;
    vector<vector<int> > C;
    int k,m,n;
//k:类的个数；m:属性个数；n:样本个数
//X:样本；centriod:记录聚类中心；C:记录每个类的样本的下标

    double **P,**R;
    double **F,*FP,F_all;
    kmeans(int kk,int nn,int mm,double *a)
    {
        //数据初始化
        k=kk;m=mm;n=nn;
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

        //初始化聚类中心
        init_centriod();
        show_centriod();

        //求解
        

        bool flag=true;
        while(flag)
        {
            cluster_assign();
            show_cluster_assign();

            flag=move_centriod();
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

    double get_accuracy(int *ans)
    {
        int sum=0;
        int flag[3];
        for(int i=0;i<k;i++)
        {
            for(int j=0;j<3;j++)
                flag[j]=0;
            for(int j=0;j<C[i].size();j++)
            {
                int t=ans[C[i][j]];
                flag[t]++;
            }
            int ma=flag[0];
            //cout<<flag[0]<<" "<<flag[1]<<" "<<flag[2]<<endl;
            for(int i=1;i<k;i++)
            {
                if(flag[i]>ma)
                {
                    sum+=ma;ma=flag[i];
                }
                else
                {
                    sum+=flag[i];
                }
                
            }
        }
        cout<<sum<<endl;
        return 1-1.0*sum/n;
    }

    double get_accuracy_f(int *ans)
    {
        cluster_assign();
        show_cluster_assign();

        int K=k;

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
            }
            F_all+=cnt[index]*FP[i];
            sum+=cnt[index];
        }
        F_all=F_all/sum;
        return F_all;
    }

};

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
    kmeans K(k,n,m,a);
    cout<<"The accuracy is "<<K.get_accuracy(ans)<<endl;
    system("pause");
    return 0;
}