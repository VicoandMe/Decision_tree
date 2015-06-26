#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <fstream>
using namespace std;
#define MAXLEN 17//输入每行的数据个数
vector <string> data;
struct Node{//决策树节点
	int attribute;//属性值
	int value;
	Node* left;
	Node* right;
	Node* mid;
	Node(){
		attribute = -1;
		left = NULL;
		right = NULL;
		mid = NULL;
		value = -1;
	}
};
Node * root;

bool input() {
	ifstream in("dataset.txt");
	if (!in.is_open()) {
		cout << "Error opening file";
		return 0;
	}
	
	string tdata;
	string get = "";
	while(getline(in,tdata)) {
		int i = 0;
		string temp;
		temp="";
		for(int index = 0; index < MAXLEN;index++) {
		  get = "";
		  while(tdata[i] != ',' && i < tdata.length()) {
		  	get.push_back(tdata[i]);
		  	i++;
		  }
		  i++;
		  if(index == 0) {
		  	if(get == "republican") {
		  	  temp.push_back('1');
		  	} else {
		  	  temp.push_back('0');
		  	}
		  } else {
		  	  if(get == "y") {
		  	  temp.push_back('1');
		  	} else if(get == "n") {
		  	  temp.push_back('0');
		  	} else {
		  	  temp.push_back('2');
		  	}
		  }
		}
		data.push_back(temp);
	}
}

int MostCommonLabel(vector <string> remain_state) {
	int p = 0, n = 0;
	for(unsigned i = 0; i < remain_state.size(); i++){
		if(remain_state[i][0] == 1) p++;
		else n++;
	}
	if(p >= n) return 1;
	else return 0;
}

bool AllTheSameLabel(vector<string> remain_state, int t) {
	for(int i = 0; i < remain_state.size(); i++) {
		if(remain_state[i][0] != (char)(t+'0')) {
			return false;
		}
	}
	return true;
}

//根据具体属性和值来计算熵
double ComputeEntropy(vector<string> remain_state, int attribute, int value,bool ifparent){
	vector<int> count (2,0);
	for(int i = 0; i < remain_state.size(); i++) {
		if(ifparent) {
			if(remain_state[i][0] == '1') {
				count[0]++;
			} else {
				count[1]++;
			}
		} else {
			if(remain_state[i][attribute] == (char)(value+'0')) {
			  if(remain_state[i][0] == '1') {
				count[0]++;
			  } else {
				count[1]++;
			  }
			}
		}
	}
	if(count[0] == 0 || count[1] == 0 ) return 0;//全部是正实例或者负实例
	//具体计算熵 根据[+count[0],-count[1]],log2为底通过换底公式换成自然数底数
	double sum = count[0] + count[1];
	double entropy = -count[0]/sum*log(count[0]/sum)/log(2.0) - count[1]/sum*log(count[1]/sum)/log(2.0);
	return entropy;
}

//计算按照属性attribute划分当前剩余实例的信息增益
double ComputeGain(vector <string> remain_state, int attribute){
	unsigned int j,k,m;
	//首先求不做划分时的熵
	double parent_entropy = ComputeEntropy(remain_state, attribute, 0, true);
	double children_entropy = 0;
	//然后求做划分后各个值的熵
	children_entropy+=ComputeEntropy(remain_state, attribute, 0, false);
	children_entropy+=ComputeEntropy(remain_state, attribute, 1, false);
	children_entropy+=ComputeEntropy(remain_state, attribute, 2, false);
	
	return (parent_entropy - children_entropy);	
}

Node * BulidDecisionTreeDFS(Node *p, vector <string> remain_state, vector <int> remain_attribute){
	if (p == NULL)
	  p = new Node();
    
	if (AllTheSameLabel(remain_state, 1)){
		p->value = 1;
		p->attribute = 0;
		return p;
	}
	
	if (AllTheSameLabel(remain_state, 0)){
		p->value = 0;
		p->attribute = 0;
		return p;
	}
	
    if(remain_attribute.size() == 0){//所有的属性均已经考虑完了,还没有分尽
	  int label = MostCommonLabel(remain_state);
	  p->attribute = 0;
	  p->value = label;
	  return p;
	}
	
	double max_gain = 0, temp_gain;
	vector <int>::iterator max_it = remain_attribute.begin();
	vector <int>::iterator it1;
	for(it1 = remain_attribute.begin(); it1 < remain_attribute.end(); it1++){
		temp_gain = ComputeGain(remain_state, (*it1));
		if(temp_gain > max_gain) {
			max_gain = temp_gain;
			max_it = it1;
		}
	}
	
	p->attribute = *max_it;
	vector<string> new_state1;
	vector<string> new_state2;
	vector<string> new_state3;
	remain_attribute.erase(max_it);
	for(int i = 0; i < remain_state.size(); i++) {
		if(remain_state[i][p->attribute] == '0') {
			new_state1.push_back(remain_state[i]);
		}
	}
	if(new_state1.size()!=0) {
	p->left = BulidDecisionTreeDFS(p->left, new_state1, remain_attribute);
    }
	for(int i = 0; i < remain_state.size(); i++) {
		if(remain_state[i][p->attribute] == '1') {
			new_state2.push_back(remain_state[i]);
		}
	}
	if(new_state2.size()!=0){
	p->right = BulidDecisionTreeDFS(p->right, new_state2, remain_attribute);
    }
	for(int i = 0; i < remain_state.size(); i++) {
		if(remain_state[i][p->attribute] == '2') {
			new_state3.push_back(remain_state[i]);
		}
	}
	if(new_state3.size()!=0){
	p->mid = BulidDecisionTreeDFS(p->mid, new_state3, remain_attribute);
    }
	return p;
}

bool check_single(Node *p, string state) {
	int count = 0;
	Node *temp = p;
	while(1) {
		if(temp->attribute == 0) {
			if((char)(temp->value+'0') == state[0]) {
				return true;
			} else {
				return false;
			}
		}
		if(state[temp->attribute] == '0') {
		  if(temp->left != NULL) {
		  	temp = temp->left;
		  } else {
		  	return false;
		  }
		} else if(state[temp->attribute] == '1'){
		  if(temp->right != NULL) {
		  	temp = temp->right;
		  } else {
		  	return false;
		  }
		} else if(state[temp->attribute] == '2'){
		  if(temp->mid != NULL) {
		  	temp = temp->mid;
		  } else {
		  	return false;
		  }
		}
	}
}
double check_decision_tree(Node *p, vector <string> check_state) {
	double itrue;
	double ifalse;
	for(int i = 0; i < check_state.size();i++) {
		if(check_single(p,check_state[i])) {
			itrue++;
		} else {
			ifalse++;
		}
	}
	return itrue/(itrue+ifalse);
}
int main() {
	vector <int> remain_attribute;
	root = NULL;
	for(int i = 1; i < 17; i++) {
		remain_attribute.push_back(i);
	}
	
	if(input()) {
	vector <string> remain_state;
	vector <string> check_state;
    for(unsigned int i = 0; i < data.size(); i++){
    	if(i < data.size() - 20) {
		  remain_state.push_back(data[i]);
	    } else {
	      check_state.push_back(data[i]);
	    }
	}
	root = BulidDecisionTreeDFS(root,remain_state,remain_attribute);
	double rate;
	rate = check_decision_tree(root,check_state);
	cout<<rate<<endl;
	}
	
} 
