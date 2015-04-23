#define _MATRIX_H
#include <iostream>
using namespace std;
class Matrix{
public:
	Matrix();
	Matrix(int, int, double*);
	Matrix(const Matrix&);

	Matrix operator+(const Matrix&);
	Matrix operator-(const Matrix&);
	Matrix operator*(const Matrix&);
	Matrix operator/(Matrix&);
	Matrix& operator=(const Matrix&);
	Matrix& operator+=(const Matrix&);
	Matrix& operator-=(const Matrix&);
	friend ostream& operator<<(ostream&, const Matrix&);

	inline double TakeElem(int i, int j)
	{
		return vlaue[(i-1)*column + j -1];
	}
	Matrix operator[](int);
	Matrix operator()(int);
	Matrix TakeSubBlock(int, int, int, int);

	Matrix DateMult(double);
	Matrix Rotate();
	Matrix Inverse();
	friend double detMatrix(const Matrix);
	~Matrix();
private:
	int row;
	int column;
	double* value;
};

Matrix::Matrix()
{
	row = 0;
	column = 0;
	value = NULL;
}
Matrix::Matrix(int r, int c, double* v):row(r), column(c)
{
	value = new double[r*c];
	for(int i = 0; i < r*c; i++)
		value[i] = v[i];
}
Matrix::Matrix(const Matrix& m)
{
	row = m.row;
	column = m.column;
	value = new double[row * column];
	for( int i = 0; i < row * column; i++)
		value[i] = m.value[i];
}
Matrix Matrix::operator+( const Matrix& m )
{
	if ( row != m.row || column != m.column ) exit(0);
	Matrix temp;
	temp.value = new double[row*column];
	temp.row = row;
	temp.column = column;
	for ( int i = 0; i < row * column; i++)
		temp.value[i] = value[i] + m.value[i];
	return temp;
}
Matrix Matrix::operator-( const Matrix& m )
{
	if ( row != m.row || column != m.column ) exit(0);
	Matrix temp;
	temp.value = new double[row*column];
	temp.row = row;
	temp.column = column;
	for ( int i = 0; i < row * column; i++)
		temp.value[i] = value[i] - m.value[i];
	return temp;
}
Matrix Matrix::operator*(const Matrix& m)
{
	if(column != m.row)  exit(0);
	Matrix temp;
	temp.value = new double[row*m.column];
	temp.row = row;
	temp.column = m.column;
	memset(temp.value, 0, sizeof(double)*row*m.column);
	temp.value[0] = 0;
	int k, count, temp1;
	k = count = temp1 = 0;
	for(int i=0; i<row; i++)
	{     
		while(k<m.column)     
		{        
			for(int j=i*column; j<(i+1)*column; j++)     
			{      
				temp.value[count] += value[j] * m.value[temp1];
				temp1 += m.column;
			}     
			k++;     
			temp1 = k;        
			count++;     
		}             
		temp1 = 0;     
		k = 0;     
	}     
	return temp;
}
Matrix Matrix::operator/(Matrix& m)
{
	return (*this)*m.Inverse();
}
Matrix& Matrix::operator=(const Matrix& m)
{
	row = m.row;
	column = m.column;
	value = new double[ row*volumn ];
	for( int i = 0; i < row * column; i++)
		value[i] = m.value[i];
	return *this;
}
Matrix& Matrix::operator+=(const Matrix& m)
{
	if (row != m.row || volumn != m.column) exit(0);
	for( int i = 0; i < row * column; i++)
		value[i] += m.value[i];
	return *this;
}
Matrix& Matrix::operator+=(const Matrix& m)
{
	if (row != m.row || volumn != m.column) exit(0);
	for( int i = 0; i < row * column; i++)
		value[i] -= m.value[i];
	return *this;
}
Matrix Matrix::operator[](int i)	//take one row
{
	int count = 0;
	Matrix temp;
	temp.row = 1;
	temp.column = column;
	temp.value = new double[ column ];
	for (int j = (i-1)*column; j < i*column; j++)
		temp.value[count++] = value[j];
	return temp;
}
Matrix Matrix::operator()(int i) 	//take one column
{
	int count = 0;
	Matrix temp;
	temp.row = row;
	temp.column = 1;
	temp.value = new double[row];
	for ( int j = i - 1; j < (row - 1)*column + i; j+=column )
		temp.value[count++] = value[j];
	return temp;
}
ostream& operator<<(ostream& o, const Matrix& m)
{
	o<<"Print the matrix as follows: "<<endl;
	for ( int i = 0; i < m.row*m.column; i++)
	{
		if ( (i+1) % m.column == 0 )
			o<<m.value[i]<<endl;
		else
			o<<m.value[i]<<" ";
	}
	return o;
}
Matrix Matrix::TakeSubBlock( int i1, int j1, int i2, int j2)
{
	if(i1 > i2 || j1 > j2)  exit(0);
	int count = 0;  
	Matrix temp;  
	temp.row = (i2-i1)+1;  
	temp.column = (j2-j1)+1;  
	temp.value = new double[temp.row * temp.column];  
	for(int i=(i1-1)*column+j1-1; i<(i2-1)*column+j2; i++)   
	{    
		temp.value[count++] = value[i];          
		if(count % temp.column == 0)       
			i += (column+j1-j2-1);      
	}  
	return temp;
}
Matrix Matrix::Rotate()
{
	Matrix temp;
	temp.row = column;
	temp.column = row;
	temp.value = new double[row*column];
	for(int i=0; i<row; i++)   
		for(int j=0; j<column; j++)       
			temp.value[j*row+i] = value[i*column+j];  
	return temp; 
}
Matrix Matrix::DateMult( double num )
{
	Matrix temp;
	temp.row = row;
	temp.column = column;
	temp.value = new double[row * column];
	for ( int i = ; i < row*column; i++ )
		temp.value[i] = num * value[i];
	return temp;
}
double detMatrix(const Matrix M)
{  
	if(M.row != M.column)  exit(0);  
	int m, n, s, t, k=1;  
	double c, x, sn, f=1;  
	for(int i=0,j=0; i<M.row&&j<M.row; i++,j++)  
	{   
		if(M.value[i*M.column+j] == 0)   
		{    
			for(m=i; M.value[m*M.column+j]==0; m++);    
				if(m == M.row)    
				{     
					sn = 0;     
					return sn;    
				}    
				else     
					for(n=j; n<M.row; n++)     
					{      
						c = M.value[i*M.column+n];      
						M.value[i*M.column+n] = M.value[m*M.column+n];
						M.value[m*M.column+n] = c;     
					}     
					k *= (-1);      
				}   
		for(s=M.row-1; s>i; s--)   
		{    
			x = M.value[s*M.column+j];    
			for(t=j; t<M.row; t++)     
				M.value[s*M.column+t] -= M.value[i*M.column+t]*(x / M.value[i*M.column+j]);   
		}  
	}  
	for(int i=0; i<M.row; i++)   
		f *= M.value[i*M.column+i];  
	sn = k * f; 
	return sn;
}
Matrix Matrix::Inverse()
{
	int count = 0;
	double var = 0;
	double var1;
	if ( (var = detMatrix(*this) ) == 0 ) exit(0);
	Matrix temp1, temp2;
	temp1.row = row;
	temp1.column = column;
	temp1.value = new double[row*column];

	temp2.row = row - 1;
	temp2.column = column - 1;
	temp2.value = new double[ (row - 1 )*(column - 1) ];

	for( int i = 0; i < row; i++ )
		for(int j = 0; j < column; j++)
		{
			int l;
			if ( (i + j) % 2 == 0 )
				l = 1;
			else
				l = -1;
			for (int k = 0; k < ( row - 1 )*( column - 1 ); )
			{
				if ( count / row == i || count % row == j)
				{
					count++;
					continue;
				}
				temp2.value[k] = value[count];
				k++;
				count++;
			}
			count = 0;
			var1 = detMatrix(temp2)*l/var;
			temp1.value[j*row + i] = var1 ? var1 : 0;
		}
		return temp1;
}
Matrix::~Matrix()
{
	if(value != NULL)
	{
		delete[] value;
		value = NULL;
	}
}