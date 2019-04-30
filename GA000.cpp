#include <iostream>
#include <fstream>
#include <cmath>
#include <cstring>
#include <time.h>
#include <algorithm>
using namespace std;

ofstream outfile;
#define MAXPARALLEL 30
#define ordernumber 5     //������
#define workpiecesnumber 7  //��������
#define populationnumber 2000  //ÿһ����Ⱥ�ĸ�����
int parallel[ordernumber];

double crossoverrate = 0.6;            //�������
double mutationrate = 0.02;             //�������
int G = 1000;                       //ѭ������100
int usetime[workpiecesnumber][ordernumber] = {
	{60,0,90,240,420},{78,150,90,0,0},{60,120,90,0,0},{60,162,0,0,0},
	{48,144,0,288,0},{60,162,18,0,0},{60,120,90,240,420}
};  //�ڼ��������ڼ�������ļӹ���ʱ��
//int usetime[workpiecesnumber][ordernumber];  //�ڼ��������ڼ�������ļӹ���ʱ��
int machinetime[ordernumber][MAXPARALLEL] = { 0 }; //�ڼ�������ĵڼ�̨���л�����ͳ��ʱ�䣻
int starttime[workpiecesnumber][ordernumber][MAXPARALLEL];//�ڼ��������ڼ��������ڵڼ�̨���л��Ͽ�ʼ�ӹ���ʱ�䣻
int finishtime[workpiecesnumber][ordernumber][MAXPARALLEL];//�ڼ��������ڼ��������ڵڼ�̨���л�����ɼӹ���ʱ�䣻
int ttime[populationnumber];      //�����makespan��                                            
int a[populationnumber][workpiecesnumber];//�ڼ�����Ⱦɫ��˳�򣬼������ӹ�˳��
int times[100];  //�����洢��֪��ʱ�����飻
int makespan;    //�ܵ����̼ӹ�ʱ�䣻
int flg7;   //��ʱ�洢���̼ӹ�ʱ�䣻
double fits[populationnumber];//�洢ÿһ����Ⱥÿһ���������Ӧ�ȣ����ڽ���ѡ�������
int tmpStore[populationnumber];//�������ÿ��������ֵ
int initialization()   //��ʼ����Ⱥ����Ⱥ�и�����ͬ ���и���ͬ
{
	for (int i = 0; i < populationnumber; i++)     //��������һ������������ȫ���еĸ��壻
		for (int j = 0; j < workpiecesnumber; j++)
		{
			a[i][j] = j + 1;
		}

	for (int i = 0; i < populationnumber; i++)     //��ȫ���еĸ��������ѡȡ��������λ�������ظ����������Σ����γ������ʼ��Ⱥ��
		for (int j = 0; j < workpiecesnumber; j++)
		{
			int flg1 = rand() % workpiecesnumber;
			int flg2 = rand() % workpiecesnumber;
			int flg3 = a[i][flg1];
			a[i][flg1] = a[i][flg2];
			a[i][flg2] = flg3;
		}
	//for (int i = 0; i < populationnumber; i++)
	//{
	//	for (int j = 0; j < workpiecesnumber; j++)
	//	{
	//		cout << a[i][j] << " ";
	//	}
	//	cout << endl;
	//}
	return 0;
}

void fitness(int c)   //������Ӧ�Ⱥ�����c����ĳ���壻
{
	int arrival[workpiecesnumber] = { 16,47,82,120,141,182,211 };
	int totaltime;      //�ܵļӹ�����ʱ�䣨makespan����
	int temp1[workpiecesnumber] = { 0 };
	int temp2[workpiecesnumber] = { 0 };
	int temp3[workpiecesnumber] = { 0 };

	for (int j = 0; j < workpiecesnumber; j++)   //temp1��ʱ�洢����c�Ļ������У��Ա���в�ͬ����֮��ļӹ�ʱ��¼�����ӹ��Ⱥ�˳��
	{
		temp1[j] = a[c][j];
		temp2[j] = arrival[a[c][j] - 1];
		temp3[j] = arrival[a[c][j] - 1];
	}

	for (int i = 0; i < ordernumber; i++)
	{
		for (int j = 0; j < workpiecesnumber; j++)  //��ѭ����Ŀ����ͨ���Ƚ����л����ĵ�ǰ����ʱ�䣬�ҳ����ȿ��еĻ����������µĹ���������
		{
			int m = machinetime[i][0];        //�ȼ�¼��i������ĵ�һ̨���л����ĵ�ǰ����ʱ�䣻
			int n = 0;
			for (int p = 0; p < parallel[i]; p++) //���������л������бȽϣ��ҳ�ʱ����С�Ļ�����
			{
				if (m > machinetime[i][p])
				{
					m = machinetime[i][p];
					n = p;
				}
			}
			int q = temp1[j];                 //��˳����ȡtemp1�еĹ����ţ��Թ������мӹ���
			starttime[q - 1][i][n] = max(machinetime[i][n], temp3[j]);  //��ʼ�ӹ�ʱ��ȡ�û����ĵ�ǰʱ��͸ù�����һ�������깤ʱ������ֵ��
			machinetime[i][n] = starttime[q - 1][i][n] + usetime[q - 1][i]; //�������ۼƼӹ�ʱ����ڻ�����ʼ�ӹ���ʱ�̣����ϸù����ӹ����õ�ʱ�䣻
			finishtime[q - 1][i][n] = machinetime[i][n];                 //�������깤ʱ����Ǹû�����ǰ���ۼƼӹ�ʱ�䣻
			temp2[j] = finishtime[q - 1][i][n];       //��ÿ���������깤ʱ�丳��temp2�������깤ʱ��Ŀ��������ھ�����һ������Ĺ����ӹ�˳��
		}
		int flg2[workpiecesnumber] = { 0 };           //������ʱ���飬���ڽ�temp1��temp2�еĹ����������У�
		for (int s = 0; s < workpiecesnumber; s++)
		{
			flg2[s] = temp1[s];
		}
		//
		for (int e = 0; e < workpiecesnumber - 1; e++)
		{
			for (int ee = 0; ee < workpiecesnumber - 1 - e; ee++) // ����temp2�洢������һ��������깤ʱ�䣬�ڽ�����һ����������ʱ���������깤��������
			{                                            //ԭ����ˣ���ѭ����Ŀ�����ڽ�temp2�а��ռӹ�ʱ���С�������У�ͬʱtemp1��Ӧ���б任
				if (temp2[ee] > temp2[ee + 1])               //����¼temp2�еĹ����ţ�
				{
					int flg5 = temp2[ee];
					int flg6 = flg2[ee];
					temp2[ee] = temp2[ee + 1];
					flg2[ee] = flg2[ee + 1];
					temp2[ee + 1] = flg5;
					flg2[ee + 1] = flg6;
				}
			}
		}
		//
		for (int e = 0; e < workpiecesnumber; e++)    //����temp1��temp2�����ݣ���ʼ��һ������
		{
			temp1[e] = flg2[e];
			temp3[e] = temp2[e];
		}
	}
	totaltime = 0;
	for (int i = 0; i < parallel[ordernumber - 1]; i++) //�Ƚ����һ������������ۼƼӹ�ʱ�䣬���ʱ����Ǹ����̵ļӹ�ʱ�䣻
		if (totaltime < machinetime[ordernumber - 1][i])
		{
			totaltime = machinetime[ordernumber - 1][i];
		}
	for (int i = 0; i < workpiecesnumber; i++)  //��������㣬������һ������ļӹ�ʱ��ͳ�ƣ�
		for (int j = 0; j < ordernumber; j++)
			for (int t = 0; t < parallel[i]; t++)
			{
				starttime[i][j][t] = 0;
				finishtime[i][j][t] = 0;
				machinetime[j][t] = 0;
			}
	makespan = totaltime;
	fits[c] = 1.000 / makespan;          //��makespanȡ������Ϊ��Ӧ�Ⱥ�����
}
void gant(int c)                   //�ú�����Ϊ�˽����Ľ�������������ʵ�չʾ�����ɸ���ͼ��������Ľ���Լ�����Ľ����û��Ӱ�죻
{
	int arrival[workpiecesnumber] = { 16,47,82,120,141,182,211 };
	int totaltime;
	char machine[ordernumber*MAXPARALLEL][500] = { "0" };
	int temp1[workpiecesnumber] = { 0 }; //�ӹ�˳��
	int temp2[workpiecesnumber] = { 0 }; //��һ��������ʱ��
	int temp3[workpiecesnumber] = { 0 };
	////////////////////////////////////////
	for (int j = 0; j < workpiecesnumber; j++)
	{
		temp1[j] = a[c][j];
		temp2[j] = arrival[a[c][j] - 1];
		temp3[j] = arrival[a[c][j] - 1];
		cout << "******" << temp1[j] << endl;
	}
	////
	//int flg2[workpiecesnumber] = { 0 };
	//for (int s = 0; s < workpiecesnumber; s++)
	//{
	//	flg2[s] = temp1[s];
	//}
	//for (int e = 0; e < workpiecesnumber - 1; e++)
	//{
	//	for (int ee = 0; ee < workpiecesnumber - 1 - e; ee++)
	//	{
	//		if (temp2[ee] > temp2[ee + 1])
	//		{
	//			int flg5 = temp2[ee];
	//			int flg6 = flg2[ee];
	//			temp2[ee] = temp2[ee + 1];
	//			flg2[ee] = flg2[ee + 1];
	//			temp2[ee + 1] = flg5;
	//			flg2[ee + 1] = flg6;
	//			//swap(temp2[ee],temp2[ee+1]);
	//			//swap(flg2[ee],flg2[ee+1]);
	//		}
	//	}
	//}
	//for (int e = 0; e < workpiecesnumber; e++)
	//{
	//	temp1[e] = flg2[e];
	//	temp3[e] = temp2[e];
	//	//cout<<"temp3=="<<temp3[e]<<endl;
	//}
	////
	for (int i = 0; i < ordernumber; i++)
	{
		for (int j = 0; j < workpiecesnumber; j++)
		{
			int m = machinetime[i][0];
			int n = 0;
			for (int p = 0; p < parallel[i]; p++) //�ҳ�ʱ����С�Ļ�����
			{
				if (m > machinetime[i][p])
				{
					m = machinetime[i][p];
					n = p;
				}
			}
			int q = temp1[j];
			starttime[q - 1][i][n] = max(machinetime[i][n], temp3[j]);
			machinetime[i][n] = starttime[q - 1][i][n] + usetime[q - 1][i];
			finishtime[q - 1][i][n] = machinetime[i][n];
			temp2[j] = finishtime[q - 1][i][n];
			cout<<"q:"<<q<<"start:"<<starttime[q-1][i][n]<<"   use:"<<usetime[q-1][i]<< "  machine_idx:"<<n <<"  machine:"<<machinetime[i][n]<<"   finish:"<<finishtime[q-1][i][n]<<endl;

			//machine��gant�����ַ�����

			for (int h = starttime[q - 1][i][n]; h < finishtime[q - 1][i][n]; h++)
			{
				//machine[i*MAXPARALLEL + n][h] = '0' + q;
				if (q >= 0 && q < 10)
					machine[i*MAXPARALLEL + n][h] = '0' + q;
				else
					machine[i*MAXPARALLEL + n][h] = 'A' + (q - 10);
			}
		}
		int flg2[workpiecesnumber] = { 0 };
		for (int s = 0; s < workpiecesnumber; s++)
		{
			flg2[s] = temp1[s];
		}
		for (int e = 0; e < workpiecesnumber - 1; e++)
		{
			for (int ee = 0; ee < workpiecesnumber - 1 - e; ee++)
			{
				if (temp2[ee] > temp2[ee + 1])
				{
					int flg5 = temp2[ee];
					int flg6 = flg2[ee];
					temp2[ee] = temp2[ee + 1];
					flg2[ee] = flg2[ee + 1];
					temp2[ee + 1] = flg5;
					flg2[ee + 1] = flg6;
					//swap(temp2[ee],temp2[ee+1]);
					//swap(flg2[ee],flg2[ee+1]);
				}
			}
		}
		for (int e = 0; e < workpiecesnumber; e++)
		{
			temp1[e] = flg2[e];
			temp3[e] = temp2[e];
			//cout<<"temp3=="<<temp3[e]<<endl;
		}
	}
	totaltime = 0;
	for (int i = 0; i < parallel[ordernumber - 1]; i++)
		if (totaltime < machinetime[ordernumber - 1][i])
		{
			totaltime = machinetime[ordernumber - 1][i];
		}
	cout << "total=" << totaltime << endl;
	outfile << totaltime << endl;///////////////////////////////////////////////////////////////////////////////
	flg7 = totaltime;

	int idx = 0;
	for (int i = 0; i < ordernumber; i++)
	{
		for (int u = 0; u < parallel[i]; u++)
		{
			for (int uu = 0; uu < 100; uu++)
			{
				outfile << machine[idx + u][uu];
				cout << machine[idx + u][uu];
			}
			outfile << endl;
			cout << endl;
		}
		idx += MAXPARALLEL;
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void select()
{
	double roulette[populationnumber + 1] = { 0.00 };	//��¼���̶ĵ�ÿһ���������䣻
	double pro_single[populationnumber];			//��¼ÿ��������ֵĸ��ʣ����������Ӧ�ȳ���������Ӧ��֮�ͣ�
	double totalfitness = 0.00;                	//��Ⱥ���и������Ӧ��֮�ͣ�
	int a1[populationnumber][workpiecesnumber];	//�洢a�����и����Ⱦɫ�壻

	for (int i = 0; i < populationnumber; i++)     		//�������и�����Ӧ�ȵ��ܺͣ�
	{
		totalfitness = totalfitness + fits[i];
	}
	for (int i = 0; i < populationnumber; i++)
	{
		pro_single[i] = fits[i] / totalfitness;   	//����ÿ��������Ӧ����������Ӧ��֮�ȣ�
		roulette[i + 1] = roulette[i] + pro_single[i]; //��ÿ������ĸ����ۼӣ��������̶ģ�
	}
	for (int i = 0; i < populationnumber; i++)
	{
		for (int j = 0; j < workpiecesnumber; j++)
		{
			a1[i][j] = a[i][j];               //a1��ʱ�洢a��ֵ��
		}
	}
	for (int i = 0; i < populationnumber; i++)
	{
		int a2;   //��ʶ�����������֮��a2��¼�������ţ�
		double p = rand() % (1000) / (double)(1000);
		for (int j = 0; j < populationnumber; j++)
		{
			if (p >= roulette[j] && p < roulette[j + 1])
				a2 = j;
		}
		for (int m = 0; m < workpiecesnumber; m++)
		{
			a[i][m] = a1[a2][m];
		}
	}
}
void crossover()
/*��Ⱥ�еĸ����������������ԣ���Գɹ�������������Ϊ����1�͸���2���н��������
�������������ͬ�Ļ����λ���Ӵ�1�̳и���2����λ֮��Ļ���Ƭ�Σ��������˳�򼯳ɸ���1��δ�ظ��Ļ���
�Ӵ�2�̳и���1����λ֮��Ļ���Ƭ�Σ��������˳�򼯳ɸ���2��δ�ظ��Ļ���*/
{
	for (int i = 0; i < populationnumber / 2; i++) //�����и���ƽ���ֳ������֣�һ����Ϊ����ĸ���1��һ����Ϊ���н���ĸ���2��
	{
		int n1 = 1 + rand() % workpiecesnumber / 2;    //�÷�������������ͬ�Ļ���λ��
		int n2 = n1 + rand() % (workpiecesnumber - n1 - 1) + 1;
		int n3 = rand() % 10;
		if (n3 == 2)   //n3=2�ĸ���Ϊ0.1��������0.1�ĸ��ʣ���ô�ͽ��н��������
		{
			int temp1[workpiecesnumber] = { 0 }; int temp2[workpiecesnumber] = { 0 };
			for (int j = 0; j < workpiecesnumber; j++)
			{
				int flg1 = 0; int flg2 = 0;
				for (int p = n1; p < n2; p++)          //�������λ֮��Ļ���Ƭ�ν��н��棬temp1��temp2��¼û�з����ظ��Ļ���
				{
					if (a[2 * i + 1][p] == a[2 * i][j])
						flg1 = 1;
				}
				if (flg1 == 0) { temp1[j] = a[2 * i][j]; }

				for (int p = n1; p < n2; p++)
				{
					if (a[2 * i][p] == a[2 * i + 1][j])
						flg2 = 1;
				}
				if (flg2 == 0) { temp2[j] = a[2 * i + 1][j]; }

			}


			for (int j = n1; j < n2; j++)             //�Ӵ�1�̳и���2�����λ֮��Ļ����Ӵ�2�̳и���1�����λ֮��Ļ���
			{
				int n4 = 0;
				n4 = a[2 * i][j];
				a[2 * i][j] = a[2 * i + 1][j];
				a[2 * i + 1][j] = n4;
			}
			for (int p = 0; p < n1; p++)               //�Ӵ�1��һ�����֮ǰ�Ļ���Ƭ�Σ���˳�����μ̳и���1��δ���Ӵ�1�ظ��Ļ���
			{
				for (int q = 0; q < workpiecesnumber; q++)
				{
					if (temp1[q] != 0)
					{
						a[2 * i][p] = temp1[q]; temp1[q] = 0;
						break;
					}
				}
			}
			for (int p = 0; p < n1; p++)               //�Ӵ�2��һ�����֮ǰ�Ļ���Ƭ�Σ���˳�����μ̳и���2��δ���Ӵ�2�ظ��Ļ���
			{
				for (int m = 0; m < workpiecesnumber; m++)
				{
					if (temp2[m] != 0)
					{
						a[2 * i + 1][p] = temp2[m]; temp2[m] = 0;
						break;
					}
				}
			}
			for (int p = n2; p < workpiecesnumber; p++)             //�Ӵ�1��2�����֮��Ļ���Ƭ�Σ���˳�����μ̳и���1��δ���Ӵ�1�ظ��Ļ���
			{
				for (int q = 0; q < workpiecesnumber; q++)
				{
					if (temp1[q] != 0)
					{
						a[2 * i][p] = temp1[q]; temp1[q] = 0;
						break;
					}

				}
			}
			for (int p = n2; p < workpiecesnumber; p++)               //�Ӵ�2��2�����֮��Ļ���Ƭ�Σ���˳�����μ̳и���2��δ���Ӵ�2�ظ��Ļ���
			{
				for (int m = 0; m < workpiecesnumber; m++)
				{
					if (temp2[m] != 0)
					{
						a[2 * i + 1][p] = temp2[m]; temp2[m] = 0;
						break;
					}
				}
			}

		}
	}


}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void mutation()  //�������Ϊ������죬���������������λ�����������������λ�ã�
{
	int n3 = rand() % 20;
	if (n3 == 2)
	{
		for (int i = 0; i < populationnumber; i++)
		{
			int b1 = rand() % workpiecesnumber;
			int b2 = rand() % workpiecesnumber;
			int b3 = a[i][b1];
			a[i][b1] = a[i][b2];
			a[i][b2] = b3;
		}
	}
}
int main()
{
	//for (int i = 0; i < workpiecesnumber; i++)
	//{
	//	usetime[i][0] = 4; usetime[i][1] = 8; usetime[i][2] = 6;
	//}
	parallel[0] = 3; parallel[1] = 8; parallel[2] = 5; parallel[3] = 3; parallel[4] = 2;
	cout << "//////////////////////////////////////////////////" << endl;
	srand(time(NULL));
	initialization();    //��ʼ����Ⱥ��
	for (int g = 0; g < G; g++)
	{
		//cout << "*******************************" << endl;
		//for (int i = 0; i < populationnumber; i++)
		//	tmpStore[i] = ttime[i];
		//sort(tmpStore, tmpStore + populationnumber);
		//for (int i = 0; i < populationnumber/10; i++)
		//	cout << tmpStore[i] << endl;
		//cout << "*******************************" << endl;

		for (int c = 0; c < populationnumber; c++)//����ÿ��������Ӧ�Ȳ�����ttime�У�
		{
			fitness(c);
			ttime[c] = makespan;
		}
		select();     //ѡ�������
		crossover();  //���������
		mutation();   //���������
	}
	for (int c = 0; c < populationnumber; c++)//����ÿ��������Ӧ�Ȳ�����ttime�У�
	{
		fitness(c);
		ttime[c] = makespan;
	}
	int flg8 = ttime[0];
	int flg9 = 0;
	for (int c = 0; c < populationnumber; c++)  //�������һ��ÿ���������Ӧ�ȣ����ҳ����Ÿ��壻
	{
		if (ttime[c] < flg8)
		{
			flg8 = ttime[c];
			flg9 = c;
		}
	}
	gant(flg9);   //�������׵�����ͼ��
	outfile.close();
	system("pause");
	return 0;
}
