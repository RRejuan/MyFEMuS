/*=========================================================================

 Program: FEMUS
 Module: ElemType
 Authors: Eugenio Aulisa
 
 Copyright (c) FEMTTU
 All rights reserved. 

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

//----------------------------------------------------------------------------
// includes :
//----------------------------------------------------------------------------

#include "GaussPoints.hpp"
#include "ElemType.hpp"
#include "Elem.hpp"
#include "NumericVector.hpp"

namespace femus {



unsigned elem_type::_refindex=1;



// square const vectors









const double *GaussLine[5]= {
  GaussLine0[0],
  GaussLine1[0],
  GaussLine2[0],
  GaussLine3[0],
  GaussLine4[0]
};

const double *GaussSquare[5]= {
  GaussSquare0[0],
  GaussSquare1[0],
  GaussSquare2[0],
  GaussSquare3[0],
  GaussSquare4[0]
};

const double *GaussTriangle[5]= {
  GaussTriangle0[0],
  GaussTriangle1[0],
  GaussTriangle2[0],
  GaussTriangle3[0],
  GaussTriangle4[0]
};


const double *GaussWedge[5]= {
  GaussWedge0[0],
  GaussWedge1[0],
  GaussWedge2[0],
  GaussWedge3[0],
  GaussWedge4[0]
};

const double *GaussTetrahedra[5]= {
  GaussTetrahedra0[0],
  GaussTetrahedra1[0],
  GaussTetrahedra2[0],
  GaussTetrahedra3[0],
  GaussTetrahedra4[0]
};


using std::cout;
using std::endl;

elem_type::~elem_type() {
  delete [] X;
  delete [] KVERT_IND;
  delete [] IND;

  delete [] prol_val;
  delete [] prol_ind;
  delete [] mem_prol_val;
  delete [] mem_prol_ind;

  delete [] phi;
  delete [] phi_memory;
  delete [] dphidxi;
  delete [] dphidxi_memory;
  delete [] dphideta;
  delete [] dphideta_memory;
  delete [] dphidzeta;
  delete [] dphidzeta_memory;
  
  delete [] d2phidxi2;
  delete [] d2phidxi2_memory;
  delete [] d2phideta2;
  delete [] d2phideta2_memory;
  delete [] d2phidzeta2;
  delete [] d2phidzeta2_memory;
  
  delete [] d2phidxideta;
  delete [] d2phidxideta_memory;
  delete [] d2phidetadzeta;
  delete [] d2phidetadzeta_memory;
  delete [] d2phidzetadxi;
  delete [] d2phidzetadxi_memory;
  
  delete pt_basis;
  
  
};

elem_type::elem_type(const char *solid, const char *order, const char *order_gauss) {
  //Gauss Point order
  int gauss_order;
  if (!strcmp(order_gauss,"zero")  || !strcmp(order_gauss,"first")) {
    gauss_order=0;
  } else if (!strcmp(order_gauss,"second") || !strcmp(order_gauss,"third") ) {
    gauss_order=1;
  } else if (!strcmp(order_gauss,"fourth") || !strcmp(order_gauss,"fifth") ) {
    gauss_order=2;
  } else if (!strcmp(order_gauss,"sixth")  || !strcmp(order_gauss,"seventh") ) {
    gauss_order=3;
  } else if (!strcmp(order_gauss,"eighth") || !strcmp(order_gauss,"ninth") ) {
    gauss_order=4;
  } else {
    cout<<order_gauss<<"is not a valid option for the Gauss points of"<<solid<<endl;
    exit(0);
  }

  if (!strcmp(order,"linear")) {
    SolType_=0;   
  } else if (!strcmp(order,"quadratic")) {
    SolType_=1;   
  } else if (!strcmp(order,"biquadratic")) {
    SolType_=2;   
  } else if (!strcmp(order,"constant")) {
    SolType_=3;   
  } else if (!strcmp(order,"disc_linear")) {
    SolType_=4;   
  }
  
  if (!strcmp(solid,"hex")) {//HEX
    
    ncf_[0]=8;
    ncf_[1]=20;
    ncf_[2]=27;
    
    if (!strcmp(order,"linear")) {
      type_=0;
      nc_=8;
      nf_=27;
      pt_basis = new hex1;//& hex_1;
    } else if (!strcmp(order,"quadratic")) {
      type_=1;
      nc_=20;
      nf_=81;
      pt_basis = new hexth;//& hex_th;
    } else if (!strcmp(order,"biquadratic")) {
      type_=2;
      nc_=27;
      nf_=125;
      pt_basis = new hex2;//& hex_2;
    } else if (!strcmp(order,"constant")) {
      type_=17;
      nc_=1;
      nf_=8;
      pt_basis = new hex0;//& hex_0;
    } else if (!strcmp(order,"disc_linear")) {
      type_=18;
      nc_=4;
      nf_=32;
      pt_basis = new hexpwl;//& hex_pwl;
    } else {
      cout<<order<<" is not a valid option for "<<solid<<endl;
      exit(0);
    }
    GaussWeight=hex_gauss::Gauss[gauss_order];
    GaussPoints=hex_gauss::GaussPoints[gauss_order];
  } else if (!strcmp(solid,"wedge")) { //WEDGE
         
    ncf_[0]=6;
    ncf_[1]=15;
    ncf_[2]=18;
       
    if (!strcmp(order,"linear")) {
      type_=3;
      nc_=6;
      nf_=18;
      pt_basis = new wedge1;//& wedge_1;
    } else if (!strcmp(order,"quadratic")) {
      type_=4;
      nc_=15;
      nf_=57;
      pt_basis = new wedgeth;//& wedge_th;
    } else if (!strcmp(order,"biquadratic")) {
      type_=5;
      nc_=18;
      nf_=75;
      pt_basis = new wedge2;//& wedge_2;
    } else {
      cout<<order<<" is not a valid option for "<<solid<<endl;
      exit(0);
    }
    
//     IND=new const int * [nc_];
//     for (int i=0; i<nc_; i++){
//       IND[i]=pt_basis->getIND(i);
//     }
//     KVERT_IND=new const int * [nf_];
//     X=new const double * [nf_];
//     for (int i=0; i<nf_; i++) {
//       KVERT_IND[i]=pt_basis->getKVERT_IND(i);
//       X[i]=pt_basis->getX(i);
//     }
     
    GaussWeight=GaussWedge[gauss_order];
    GaussPoints=GaussPointsWedge[gauss_order];
  } else if (!strcmp(solid,"tet")) { //TETRAHEDRA
         
    ncf_[0]=4;
    ncf_[1]=10;
    ncf_[2]=10;
    
    if (!strcmp(order,"linear")) {
      type_=6;
      nc_=4;
      nf_=10;
      pt_basis = new tet1;//& tet_1;
    } else if (!strcmp(order,"biquadratic")) {
      type_=7;
      nc_=10;
      nf_=35;
      pt_basis = new tet2;//& tet_2;
    } else if (!strcmp(order,"constant")) {
      type_=8;
      nc_=1;
      nf_=8;
      pt_basis = new tet0;//& tet_0;
    } else {
      cout<<order<<" is not a valid option for "<<solid<<endl;
      exit(0);
    }

//     IND=new const int * [nc_];
//     for (int i=0; i<nc_; i++){
//       IND[i]=pt_basis->getIND(i);
//     }
//     KVERT_IND=new const int * [nf_];
//     X=new const double * [nf_];
//     for (int i=0; i<nf_; i++) {
//       KVERT_IND[i]=pt_basis->getKVERT_IND(i);
//       X[i]=pt_basis->getX(i);
//     }
    
    GaussWeight=GaussTetrahedra[gauss_order];
    GaussPoints=GaussPointsTetrahedra[gauss_order];
  } else if (!strcmp(solid,"quad")) { //QUAD
    ncf_[0]=4;
    ncf_[1]=8;
    ncf_[2]=9;
    if (!strcmp(order,"linear")) {
      type_=8;
      nc_=4;
      nf_=9;
      pt_basis = new quad1;//& quad_1;
    } else if (!strcmp(order,"quadratic")) {
      type_=9;
      nc_=8;
      nf_=21;
      pt_basis = new quadth;//& quad_th;
    } else if (!strcmp(order,"biquadratic")) {
      type_=10;
      nc_=9;
      nf_=25;
      pt_basis = new quad2;//& quad_2;
    } else if (!strcmp(order,"constant")) {
      type_=15;
      nc_=1;
      nf_=4;
      pt_basis = new quad0;//& quad_0;
    } else if (!strcmp(order,"disc_linear")) {
      type_=16;
      nc_=3;
      nf_=12;
      pt_basis = new quadpwl;//& quad_pwl;
    } else {
      cout<<order<<" is not a valid option for "<<solid<<endl;
      exit(0);
    }
    
//     IND=new const int * [nc_];
//     for (int i=0; i<nc_; i++){
//       IND[i]=pt_basis->getIND(i);
//     }
//     KVERT_IND=new const int * [nf_];
//     X=new const double * [nf_];
//     for (int i=0; i<nf_; i++) {
//       KVERT_IND[i]=pt_basis->getKVERT_IND(i);
//       X[i]=pt_basis->getX(i);
//     }
    
    GaussWeight=GaussSquare[gauss_order];
    GaussPoints=GaussPointsSquare[gauss_order];
  } else if (!strcmp(solid,"tri")) { //TRIANGLE
    
    ncf_[0]=3;
    ncf_[1]=6;
    ncf_[2]=6;
    
    if (!strcmp(order,"linear")) {
      type_=11;
      nc_=3;
      nf_=6;
      pt_basis = new tri1;//& tri_1;
    } else if (!strcmp(order,"biquadratic")) {
      type_=12;
      nc_=6;
      nf_=15;
      pt_basis = new tri2;//& tri_2;
    } else if (!strcmp(order,"constant")) {
      type_=13; // TODO is the choice of this number ok ?????
      nc_=1;
      nf_=4;
      pt_basis = new tri0;//& tri_0;
    } else {
      cout<<order<<" is not a valid option for "<<solid<<endl;
      exit(0);
    }

//     IND=new const int * [nc_];
//     for (int i=0; i<nc_; i++){
//       IND[i]=pt_basis->getIND(i);
//     }
//     KVERT_IND=new const int * [nf_];
//     X=new const double * [nf_];
//     for (int i=0; i<nf_; i++) {
//       KVERT_IND[i]=pt_basis->getKVERT_IND(i);
//       X[i]=pt_basis->getX(i);
//     }
        
    GaussWeight=GaussTriangle[gauss_order];
    GaussPoints=GaussPointsTriangle[gauss_order];
  }

  else if (!strcmp(solid,"line")) { //line
    
    ncf_[0]=2;
    ncf_[1]=3;
    ncf_[2]=3;
    
    if (!strcmp(order,"linear")) {
      type_=13;
      nc_=2;
      nf_=3;
      pt_basis = new line1;//& line_1;
    } else if (!strcmp(order,"biquadratic")) {
      type_=14;
      nc_=3;
      nf_=5;
      pt_basis = new line2;//& line_2;
    } else if (!strcmp(order,"constant")) {
      type_=15;  // TODO is the choice of this number ok ?????
      nc_=1;   
      nf_=2;   
      pt_basis = new line0;//& line_0;
    }
    else {
      cout<<order<<" is not a valid option for "<<solid<<endl;
      exit(0);
    }

//     IND=new const int * [nc_];
//     for (int i=0; i<nc_; i++)
//       IND[i]=LINE_IND[i];
// 
//     KVERT_IND=new const int * [nf_];
//     X=new const double * [nf_];
//     for (int i=0; i<nf_; i++) {
//       KVERT_IND[i]=LINE_KVERT_IND[i];
//       X[i]=LINE_X[i];
//     }
    GaussWeight=GaussLine[gauss_order];
    GaussPoints=GaussPointsLine[gauss_order];
  } else {
    cout<<solid<<" is not a valid option"<<endl;
    exit(0);
  }

  IND=new const int * [nc_];
  for (int i=0; i<nc_; i++){
    IND[i]=pt_basis->getIND(i);
  }
  KVERT_IND=new const int * [nf_];
  X=new const double * [nf_];
  for (int i=0; i<nf_; i++) {
    KVERT_IND[i]=pt_basis->getKVERT_IND(i);
    X[i]=pt_basis->getX(i);
  }
    
  int counter=0;
  for (int i=0; i<nf_; i++) {
    for (int j=0; j<nc_; j++) {
      double phi=pt_basis->eval_phi(IND[j],X[i]);
      if (type_==16) {
        if (i/4==1) phi=pt_basis->eval_dphidx(IND[j],X[i]);
        else if (i/4==2) phi=pt_basis->eval_dphidy(IND[j],X[i]);
      }
      if (phi!=0){
        counter++;
      }
    }
  }
  double *pt_d;
  int *pt_i;

  prol_val=new double * [nf_+1];
  prol_ind=new int * [nf_+1];
  mem_prol_val=new double [counter];
  mem_prol_ind=new int [counter];

  pt_d=mem_prol_val;
  pt_i= mem_prol_ind;
  for (int i=0; i<nf_; i++) {
    prol_val[i]=pt_d;
    prol_ind[i]=pt_i;
    for (int j=0; j<nc_; j++) {
      double phi=pt_basis->eval_phi(IND[j],X[i]);
      if (type_==16) {
        if (i/4==1)
          phi=pt_basis->eval_dphidx(IND[j],X[i])/2.;
        else if (i/4==2)
          phi=pt_basis->eval_dphidy(IND[j],X[i])/2.;
      } else if (type_==18) {
        if (i/8==1)
          phi=pt_basis->eval_dphidx(IND[j],X[i])/2.;
        else if (i/8==2)
          phi=pt_basis->eval_dphidy(IND[j],X[i])/2.;
        else if (i/8==3)
          phi=pt_basis->eval_dphidz(IND[j],X[i])/2.;
      }
      if (phi!=0) {
        *(pt_d++)=phi;
        *(pt_i++)=j;
      }
    }
  }
  
  prol_val[nf_]=pt_d;
  prol_ind[nf_]=pt_i;


//   rest_val=new double * [nc_+1];
//   rest_ind=new int * [nc_+1];
//   mem_rest_val=new double [counter];
//   mem_rest_ind=new int [counter];
//
//   pt_d=mem_rest_val;
//   pt_i=mem_rest_ind;
//
//   for(int i=0;i<nc_;i++){
//     rest_val[i]=pt_d;
//     rest_ind[i]=pt_i;
//     for (int j=0;j<nf_;j++){
//       double phi=pt_basis->eval_phi(IND[i],X[j]);
//       if(type_==16){
// 	if(j/4==1) phi=pt_basis->eval_dphidx(IND[i],X[j]);
// 	else if(j/4==2) phi=pt_basis->eval_dphidy(IND[i],X[j]);
//       }
//       if(phi!=0){
// 	*(pt_d++)=phi;
// 	*(pt_i++)=j;
//       }
//     }
//   }
//   rest_val[nc_]=pt_d;
//   rest_ind[nc_]=pt_i;

  phi= new double*[GaussPoints];
  dphidxi  = new double*[GaussPoints];
  dphideta = new double*[GaussPoints];
  dphidzeta= new double*[GaussPoints];
  
  d2phidxi2  = new double*[GaussPoints];
  d2phideta2 = new double*[GaussPoints];
  d2phidzeta2= new double*[GaussPoints];
  
  d2phidxideta  = new double*[GaussPoints];
  d2phidetadzeta = new double*[GaussPoints];
  d2phidzetadxi= new double*[GaussPoints];

  phi_memory=new double [GaussPoints*nc_];
  dphidxi_memory  =new double [GaussPoints*nc_];
  dphideta_memory =new double [GaussPoints*nc_];
  dphidzeta_memory=new double [GaussPoints*nc_];
  
  d2phidxi2_memory  =new double [GaussPoints*nc_];
  d2phideta2_memory =new double [GaussPoints*nc_];
  d2phidzeta2_memory=new double [GaussPoints*nc_];
  
  d2phidxideta_memory  =new double [GaussPoints*nc_];
  d2phidetadzeta_memory =new double [GaussPoints*nc_];
  d2phidzetadxi_memory=new double [GaussPoints*nc_];

  for (unsigned i=0; i<GaussPoints; i++) {
    phi[i]=&phi_memory[i*nc_];
    dphidxi[i]  =&dphidxi_memory[i*nc_];
    dphideta[i] =&dphideta_memory[i*nc_];
    dphidzeta[i]=&dphidzeta_memory[i*nc_];
    
    d2phidxi2[i]  =&d2phidxi2_memory[i*nc_];
    d2phideta2[i] =&d2phideta2_memory[i*nc_];
    d2phidzeta2[i]=&d2phidzeta2_memory[i*nc_];
    
    d2phidxideta[i]  = &d2phidxideta_memory[i*nc_];
    d2phidetadzeta[i]= &d2phidetadzeta_memory[i*nc_];
    d2phidzetadxi[i] = &d2phidzetadxi_memory[i*nc_];
    
  }

  const double *ptx=GaussWeight+GaussPoints,*pty=GaussWeight+2*GaussPoints,*ptz=GaussWeight+3*GaussPoints;
  for (unsigned i=0; i<GaussPoints; i++,ptx++,pty++,ptz++) {
    double x[3];
    x[0]=*ptx;
    x[1]=*pty;
    x[2]=*ptz;    
    
    for (int j=0; j<nc_; j++) {
      phi[i][j] = pt_basis->eval_phi(IND[j],x); 	
      dphidxi[i][j] = pt_basis->eval_dphidx(IND[j],x); 	
      dphideta[i][j] = pt_basis->eval_dphidy(IND[j],x); 	
      dphidzeta[i][j] = pt_basis->eval_dphidz(IND[j],x); 	
      d2phidxi2[i][j] = pt_basis->eval_d2phidx2(IND[j],x);
      d2phideta2[i][j] = pt_basis->eval_d2phidy2(IND[j],x);
      d2phidzeta2[i][j] = pt_basis->eval_d2phidz2(IND[j],x);
      d2phidxideta[i][j] = pt_basis->eval_d2phidxdy(IND[j],x);
      d2phidetadzeta[i][j] = pt_basis->eval_d2phidydz(IND[j],x);
      d2phidzetadxi[i][j] = pt_basis->eval_d2phidzdx(IND[j],x);
    }
  }
}


//----------------------------------------------------------------------------------------------------
// build matrix sparsity pattern size and build prolungator matrix for the LsysPde  Matrix
//-----------------------------------------------------------------------------------------------------

void elem_type::BuildProlongation(const LinearEquation &lspdef,const LinearEquation &lspdec, const int& ielc, SparseMatrix* Projmat, 
				  const unsigned &index_sol, const unsigned &kkindex_sol) const {
  vector<int> cols(27);
   
  for (int i=0; i<nf_; i++) {
    int i0=KVERT_IND[i][0]; //id of the subdivision of the fine element
    int ielf=lspdec._msh->el->GetChildElement(ielc,i0);
    int i1=KVERT_IND[i][1]; //local id node on the subdivision of the fine element
    int iadd=lspdef._msh->el->GetDof(ielf,i1,type_);
    int irow=lspdef.GetKKDof(index_sol,kkindex_sol,iadd);  //  local-id to dof 
    int ncols=prol_ind[i+1]-prol_ind[i];
    cols.assign(ncols,0);
    for (int k=0; k<ncols; k++) {
      int j=prol_ind[i][k]; 
      int jadd=lspdec._msh->el->GetDof(ielc,j,type_);
      int jj=lspdec.GetKKDof(index_sol,kkindex_sol,jadd); 
      cols[k]=jj;
    }
    Projmat->insert_row(irow,ncols,cols,prol_val[i]);
  }
}


void elem_type::GetSparsityPatternSize(const LinearEquation &lspdef,const LinearEquation &lspdec, const int& ielc,  
				       NumericVector* NNZ_d, NumericVector* NNZ_o,
				       const unsigned &index_sol, const unsigned &kkindex_sol) const {
     
  for (int i=0; i<nf_; i++) {
    int i0=KVERT_IND[i][0]; //id of the subdivision of the fine element
    int ielf=lspdec._msh->el->GetChildElement(ielc,i0);
    int i1=KVERT_IND[i][1]; //local id node on the subdivision of the fine element
    int iadd=lspdef._msh->el->GetDof(ielf,i1,type_);
    int irow=lspdef.GetKKDof(index_sol,kkindex_sol,iadd);  //  local-id to dof 
    
    int iproc=0;
    while (irow < lspdef.KKoffset[0][iproc] || irow >= lspdef.KKoffset[lspdef.KKIndex.size()-1][iproc] ) iproc++;
    int ncols=prol_ind[i+1]-prol_ind[i];
    
    int counter_o=0;
    for (int k=0; k<ncols; k++) {
      int j=prol_ind[i][k]; 
      int jadd=lspdec._msh->el->GetDof(ielc,j,type_);
      int jcolumn=lspdec.GetKKDof(index_sol,kkindex_sol,jadd); 
      if(jcolumn < lspdec.KKoffset[0][iproc] || jcolumn >= lspdec.KKoffset[lspdef.KKIndex.size()-1][iproc] ) counter_o++;
    }
       
    NNZ_d->set(irow,ncols-counter_o);
    NNZ_o->set(irow,counter_o);
    
  }
}

void elem_type::BuildRestrictionTranspose(const LinearEquation &lspdef,const LinearEquation &lspdec, const int& ielc, SparseMatrix* Projmat, 
					  const unsigned &index_sol, const unsigned &kkindex_sol, const bool &TestDisp) const {
  vector<int> cols(27);
  bool fluid_region = (2==lspdec._msh->el->GetElementMaterial(ielc))?1:0;
  
  vector <double> copy_prol_val;
  copy_prol_val.reserve(27); 
  for (int i=0; i<nf_; i++) {
    int i0=KVERT_IND[i][0]; //id of the subdivision of the fine element
    int ielf=lspdec._msh->el->GetChildElement(ielc,i0);
    int i1=KVERT_IND[i][1]; //local id node on the subdivision of the fine element
    int iadd=lspdef._msh->el->GetDof(ielf,i1,type_);
        
    int irow=lspdef.GetKKDof(index_sol,kkindex_sol,iadd);  //  local-id to dof 
    int ncols=prol_ind[i+1]-prol_ind[i];
    
    bool isolidmark=lspdef._msh->el->GetNodeRegion(iadd);
    
    cols.assign(ncols,0);
    copy_prol_val.resize(ncols);
    for (int k=0; k<ncols; k++) {
      int j=prol_ind[i][k]; 
      int jadd=lspdec._msh->el->GetDof(ielc,j,type_);
      int jcolumn=lspdec.GetKKDof(index_sol,kkindex_sol,jadd); 
      cols[k]=jcolumn;
      
      bool jsolidmark=lspdef._msh->el->GetNodeRegion(jadd); 
      
      copy_prol_val[k]=(!TestDisp || !fluid_region || isolidmark==jsolidmark)?prol_val[i][k]:0.;
    }    
    Projmat->insert_row(irow,ncols,cols,&copy_prol_val[0]);
  }
}



//----------------------------------------------------------------------------------------------------
// build matrix sparsity pattern size and build prolungator matrix for single solution
//-----------------------------------------------------------------------------------------------------

void elem_type::GetSparsityPatternSize(const mesh &meshf,const mesh &meshc, const int& ielc, NumericVector* NNZ_d, NumericVector* NNZ_o) const {
   
  for (int i=0; i<nf_; i++) {
    int i0=KVERT_IND[i][0]; //id of the subdivision of the fine element
    int ielf=meshc.el->GetChildElement(ielc,i0);
    int i1=KVERT_IND[i][1]; //local id node on the subdivision of the fine element
    int iadd=meshf.el->GetDof(ielf,i1,type_);
    int irow=meshf.GetMetisDof(iadd,SolType_);  //  local-id to dof
    int iproc=0;
    while (irow < meshf.MetisOffset[SolType_][iproc] || irow >= meshf.MetisOffset[SolType_][iproc+1] ) iproc++;
    
    int ncols=prol_ind[i+1]-prol_ind[i];
    unsigned counter_o=0;
    for (int k=0; k<ncols; k++) {
      int j=prol_ind[i][k]; 
      int jadd=meshc.el->GetDof(ielc,j,type_);
      int jcolumn=meshc.GetMetisDof(jadd,SolType_);
      if(jcolumn < meshc.MetisOffset[SolType_][iproc] || jcolumn >= meshc.MetisOffset[SolType_][iproc+1] ) counter_o++;
    }
       
    NNZ_d->set(irow,ncols-counter_o);
    NNZ_o->set(irow,counter_o);
    
  }
}

void elem_type::BuildProlongation(const mesh &meshf,const mesh &meshc, const int& ielc,
				  SparseMatrix* Projmat) const {
  vector<int> cols(27);
  for (int i=0; i<nf_; i++) {
    int i0=KVERT_IND[i][0]; //id of the subdivision of the fine element
    int ielf=meshc.el->GetChildElement(ielc,i0);
    int i1=KVERT_IND[i][1]; //local id node on the subdivision of the fine element
    int iadd=meshf.el->GetDof(ielf,i1,type_);
    int irow=meshf.GetMetisDof(iadd,SolType_);  //  local-id to dof 
    int ncols=prol_ind[i+1]-prol_ind[i];
    cols.assign(ncols,0);
    for (int k=0; k<ncols; k++) {
      int j=prol_ind[i][k]; 
      int jadd=meshc.el->GetDof(ielc,j,type_);
      int jcolumn=meshc.GetMetisDof(jadd,SolType_);
      cols[k]=jcolumn;
    }

    Projmat->insert_row(irow,ncols,cols,prol_val[i]);
  }
}

//----------------------------------------------------------------------------------------------------
// prolungator for solution printing
//----------------------------------------------------------------------------------------------------

void elem_type::GetSparsityPatternSize(const mesh& mesh,const int& iel, NumericVector* NNZ_d, NumericVector* NNZ_o, const unsigned &itype) const{
  for (int i=0; i<ncf_[itype]; i++) {
    int inode=mesh.el->GetDof(iel,i,type_);
    int irow=mesh.GetMetisDof(inode,itype);
    int iproc=0;
    while (irow < mesh.MetisOffset[itype][iproc] || irow >= mesh.MetisOffset[itype][iproc+1] ) iproc++;
    int ncols=prol_ind[i+1]-prol_ind[i];
    unsigned counter_o=0;
    for (int k=0; k<ncols; k++) {
      int jj=prol_ind[i][k];
      int jnode   = mesh.el->GetDof(iel,jj,type_);
      int jcolumn = mesh.GetMetisDof(jnode,SolType_);
      if(jcolumn < mesh.MetisOffset[SolType_][iproc] || jcolumn >= mesh.MetisOffset[SolType_][iproc+1] ) counter_o++;
    }
    NNZ_d->set(irow,ncols-counter_o);
    NNZ_o->set(irow,counter_o);
  }
}


void elem_type::BuildProlongation(const mesh& mesh,const int& iel, SparseMatrix* Projmat,const unsigned &itype) const{
  vector<int> cols(27);
  for (int i=0; i<ncf_[itype]; i++) {
    int inode=mesh.el->GetDof(iel,i,type_);
    int irow=mesh.GetMetisDof(inode,itype);
    int ncols=prol_ind[i+1]-prol_ind[i];
    cols.assign(ncols,0);
    for (int k=0; k<ncols; k++) {
      int jj=prol_ind[i][k];
      int jnode=mesh.el->GetDof(iel,jj,type_);
      int jcolumn=mesh.GetMetisDof(jnode,SolType_);
      cols[k]=jcolumn;
    }
    Projmat->insert_row(irow,ncols,cols,prol_val[i]);
  }
}

//---------------------------------------------------------------------------------------------------------
double* elem_type::GetPhi(const unsigned &ig) const {
  return phi[ig];
}

double* elem_type::GetDPhiDXi(const unsigned &ig) const {
  return dphidxi[ig];
}

double* elem_type::GetDPhiDEta(const unsigned &ig) const {
  return dphideta[ig];
}

double* elem_type::GetDPhiDZeta(const unsigned &ig) const {
  return dphidzeta[ig];
}


//---------------------------------------------------------------------------------------------------------
void elem_type::GetArea(const double *vtx,const double *vty, const double *vtz, const unsigned &ig,
                        double &Weight, double *other_phi) const {

  double Jac[2][3];
  for (double *pt_d=Jac[0]; pt_d<Jac[0]+6; pt_d++) *pt_d=0.;
  const double *dfx=dphidxi[ig];
  const double *dfy=dphideta[ig];
  const double *vx=vtx;
  const double *vy=vty;
  const double *vz=vtz;
  for (int inode=0; inode<nc_; inode++,dfx++,dfy++,vx++,vy++,vz++) {
    double *pt_d=Jac[0];
    *(pt_d++)+=(*dfx)*(*vx);
    *(pt_d++)+=(*dfx)*(*vy);
    *(pt_d++)+=(*dfx)*(*vz);
    *(pt_d++)+=(*dfy)*(*vx);
    *(pt_d++)+=(*dfy)*(*vy);
    *(pt_d++)+=(*dfy)*(*vz);
  }

  double det1=Jac[0][1]*Jac[1][2]-Jac[1][1]*Jac[0][2];
  double det2=Jac[0][0]*Jac[1][2]-Jac[1][0]*Jac[0][2];
  double det3=Jac[0][0]*Jac[1][1]-Jac[1][0]*Jac[0][1];
  double det=sqrt(det1*det1+det2*det2+det3*det3);

  Weight=det*GaussWeight[ig];
  double *fi=phi[ig];
  for (int inode=0; inode<nc_; inode++,other_phi++,fi++) {
    *other_phi=*fi;
  }
}


void elem_type_1D::Jacobian_AD(const vector < vector < adept::adouble > > &vt,const unsigned &ig, adept::adouble &Weight, 
			      vector < double > &other_phi, vector < adept::adouble > &gradphi, vector < adept::adouble > &nablaphi) const{
  other_phi.resize(nc_);
  gradphi.resize(nc_*1);
  nablaphi.resize(nc_*1);				
  
  adept::adouble Jac=0.;
  adept::adouble JacI;
  
  const double *dxi=dphidxi[ig];
  
  for (int inode=0; inode<nc_; inode++,dxi++) {
    Jac+=(*dxi)*vt[0][inode];
  }
    
  Weight=Jac*GaussWeight[ig];

  JacI=1/Jac;
  
  dxi = dphidxi[ig];
  const double *dxi2 = d2phidxi2[ig];
  
  for (int inode=0; inode<nc_; inode++,dxi++, dxi2++) {
    other_phi[inode]=phi[ig][inode];
    gradphi[inode]=(*dxi)*JacI;
    nablaphi[inode] = (*dxi2)*JacI*JacI;
  }
  
}

void elem_type_1D::Jacobian(const vector < vector < double > > &vt,const unsigned &ig, double &Weight, 
    			    vector < double > &other_phi, vector < double > &gradphi, vector < double > &nablaphi) const{
  other_phi.resize(nc_);
  gradphi.resize(nc_*1);
  nablaphi.resize(nc_*1);				
  
  double Jac=0.;
  double JacI;
  
  const double *dxi=dphidxi[ig];
  
  for (int inode=0; inode<nc_; inode++,dxi++) {
    Jac+=(*dxi)*vt[0][inode];
  }
    
  Weight=Jac*GaussWeight[ig];

  JacI=1/Jac;
  
  dxi = dphidxi[ig];
  const double *dxi2 = d2phidxi2[ig];
  
  for (int inode=0; inode<nc_; inode++,dxi++, dxi2++) {
    other_phi[inode]=phi[ig][inode];
    gradphi[inode]=(*dxi)*JacI;
    nablaphi[inode] = (*dxi2)*JacI*JacI;
  }
  
}

void elem_type_1D::JacobianSur_AD(const vector < vector < adept::adouble > > &vt, const unsigned &ig, adept::adouble &Weight, 
				  vector < double > &other_phi, vector < adept::adouble > &gradphi, vector < adept::adouble > &normal) const {

  other_phi.resize(nc_);
  normal.resize(2);				
			
  adept::adouble Jac[2][2]={{0.,0.},{0.,0.}};
  adept::adouble JacI[2][2];

  const double *dfeta=dphidxi[ig];

  for (int inode=0; inode<nc_; inode++,dfeta++) {
    Jac[0][0] += (*dfeta)*vt[0][inode];
    Jac[1][0] += (*dfeta)*vt[1][inode];
  }

//   normal module
  adept::adouble modn = sqrt(Jac[0][0]*Jac[0][0] + Jac[1][0]*Jac[1][0]);

  normal[0] =  Jac[1][0]/modn;
  normal[1] = -Jac[0][0]/modn;
  
  //The derivative of x with respect to eta (dx/deta) has the opposite sign with respect to the normal
  //obtained as cross product between (dx/deta , dy/deta, 0) x (0,0,1)
  //The Jacobian has the structure
  // |dx/deta  -nx|
  // |dy/deta  -ny|
  Jac[0][1] = -normal[0];
  Jac[1][1] = -normal[1];

  //The determinant of that matrix is the area
  adept::adouble det= (Jac[0][0]*Jac[1][1]-Jac[0][1]*Jac[1][0]);

  JacI[0][0] =  Jac[1][1]/det;
  JacI[0][1] = -Jac[0][1]/det;
  JacI[1][0] = -Jac[1][0]/det;
  JacI[1][1] =  Jac[0][0]/det;

  Weight = det*GaussWeight[ig];
  
  for(int inode=0;inode<nc_;inode++){
    other_phi[inode]=phi[ig][inode];
  }

}

void elem_type_1D::JacobianSur(const vector < vector < double > > &vt, const unsigned &ig, double &Weight, 
			       vector < double > &other_phi, vector < double > &gradphi, vector < double > &normal) const {

  other_phi.resize(nc_);
  normal.resize(2);				
			
  double Jac[2][2]={{0.,0.},{0.,0.}};
  double JacI[2][2];

  const double *dfeta=dphidxi[ig];

  for (int inode=0; inode<nc_; inode++,dfeta++) {
    Jac[0][0] += (*dfeta)*vt[0][inode];
    Jac[1][0] += (*dfeta)*vt[1][inode];
  }

  //   normal module
  double modn = sqrt(Jac[0][0]*Jac[0][0] + Jac[1][0]*Jac[1][0]);

  normal[0] =  Jac[1][0]/modn;
  normal[1] = -Jac[0][0]/modn;
  
  //The derivative of x with respect to eta (dx/deta) has the opposite sign with respect to the normal
  //obtained as cross product between (dx/deta , dy/deta, 0) x (0,0,1)
  //The Jacobian has the structure
  // |dx/deta  -nx|
  // |dy/deta  -ny|
  Jac[0][1] = -normal[0];
  Jac[1][1] = -normal[1];

  //The determinant of that matrix is the area
  double det= (Jac[0][0]*Jac[1][1]-Jac[0][1]*Jac[1][0]);

  JacI[0][0] =  Jac[1][1]/det;
  JacI[0][1] = -Jac[0][1]/det;
  JacI[1][0] = -Jac[1][0]/det;
  JacI[1][1] =  Jac[0][0]/det;

  Weight = det*GaussWeight[ig];
  
  for(int inode=0;inode<nc_;inode++){
    other_phi[inode]=phi[ig][inode];
  }

}

//---------------------------------------------------------------------------------------------------------
void elem_type_2D::Jacobian_AD(const vector < vector < adept::adouble > > &vt,const unsigned &ig, adept::adouble &Weight, 
			      vector < double > &other_phi, vector < adept::adouble > &gradphi, vector < adept::adouble > &nablaphi) const{
	
  other_phi.resize(nc_);
  gradphi.resize(nc_*2);
  nablaphi.resize(nc_*3);
				
  adept::adouble Jac[2][2]={{0,0},{0,0}};
  adept::adouble JacI[2][2];
  const double *dxi=dphidxi[ig];
  const double *deta=dphideta[ig];
  for (int inode=0; inode<nc_; inode++,dxi++,deta++){
    Jac[0][0]+=(*dxi)*vt[0][inode];
    Jac[0][1]+=(*dxi)*vt[1][inode];
    Jac[1][0]+=(*deta)*vt[0][inode];
    Jac[1][1]+=(*deta)*vt[1][inode];
  }
  adept::adouble det=(Jac[0][0]*Jac[1][1]-Jac[0][1]*Jac[1][0]);

  JacI[0][0]= Jac[1][1]/det;
  JacI[0][1]=-Jac[0][1]/det;
  JacI[1][0]=-Jac[1][0]/det;
  JacI[1][1]= Jac[0][0]/det;

  Weight=det*GaussWeight[ig];
     
  dxi=dphidxi[ig];
  deta=dphideta[ig];
  
  const double *dxi2=d2phidxi2[ig];
  const double *deta2=d2phideta2[ig];
  const double *dxideta=d2phidxideta[ig];
    
  for (int inode=0; inode<nc_; inode++, dxi++, deta++, dxi2++, deta2++, dxideta++) {
    
    other_phi[inode]=phi[ig][inode];
     
    gradphi[2*inode+0]=(*dxi)*JacI[0][0] + (*deta)*JacI[0][1];
    gradphi[2*inode+1]=(*dxi)*JacI[1][0] + (*deta)*JacI[1][1];
    
    nablaphi[3*inode+0]= 
      ( (*dxi2)   *JacI[0][0] + (*dxideta)*JacI[0][1] ) * JacI[0][0] +
      ( (*dxideta)*JacI[0][0] + (*deta2)  *JacI[0][1] ) * JacI[0][1]; 
    nablaphi[3*inode+1]= 
      ( (*dxi2)   *JacI[1][0] + (*dxideta)*JacI[1][1] ) * JacI[1][0] +
      ( (*dxideta)*JacI[1][0] + (*deta2)  *JacI[1][1] ) * JacI[1][1]; 
    nablaphi[3*inode+2]= 
      ( (*dxi2)   *JacI[0][0] + (*dxideta)*JacI[0][1] ) * JacI[1][0] +
      ( (*dxideta)*JacI[0][0] + (*deta2)  *JacI[0][1] ) * JacI[1][1]; 
     
  }
}

void elem_type_2D::Jacobian(const vector < vector < double > > &vt,const unsigned &ig, double &Weight, 
			    vector < double > &other_phi, vector < double > &gradphi, vector < double > &nablaphi) const{
	
  other_phi.resize(nc_);
  gradphi.resize(nc_*2);
  nablaphi.resize(nc_*3);
				
  double Jac[2][2]={{0,0},{0,0}};
  double JacI[2][2];
  const double *dxi=dphidxi[ig];
  const double *deta=dphideta[ig];
  for (int inode=0; inode<nc_; inode++,dxi++,deta++){
    Jac[0][0]+=(*dxi)*vt[0][inode];
    Jac[0][1]+=(*dxi)*vt[1][inode];
    Jac[1][0]+=(*deta)*vt[0][inode];
    Jac[1][1]+=(*deta)*vt[1][inode];
  }
  double det=(Jac[0][0]*Jac[1][1]-Jac[0][1]*Jac[1][0]);

  JacI[0][0]= Jac[1][1]/det;
  JacI[0][1]=-Jac[0][1]/det;
  JacI[1][0]=-Jac[1][0]/det;
  JacI[1][1]= Jac[0][0]/det;

  Weight=det*GaussWeight[ig];
     
  dxi=dphidxi[ig];
  deta=dphideta[ig];
  
  const double *dxi2=d2phidxi2[ig];
  const double *deta2=d2phideta2[ig];
  const double *dxideta=d2phidxideta[ig];
    
  for (int inode=0; inode<nc_; inode++, dxi++, deta++, dxi2++, deta2++, dxideta++) {
    
    other_phi[inode]=phi[ig][inode];
     
    gradphi[2*inode+0]=(*dxi)*JacI[0][0] + (*deta)*JacI[0][1];
    gradphi[2*inode+1]=(*dxi)*JacI[1][0] + (*deta)*JacI[1][1];
    
    nablaphi[3*inode+0]= 
      ( (*dxi2)   *JacI[0][0] + (*dxideta)*JacI[0][1] ) * JacI[0][0] +
      ( (*dxideta)*JacI[0][0] + (*deta2)  *JacI[0][1] ) * JacI[0][1]; 
    nablaphi[3*inode+1]= 
      ( (*dxi2)   *JacI[1][0] + (*dxideta)*JacI[1][1] ) * JacI[1][0] +
      ( (*dxideta)*JacI[1][0] + (*deta2)  *JacI[1][1] ) * JacI[1][1]; 
    nablaphi[3*inode+2]= 
      ( (*dxi2)   *JacI[0][0] + (*dxideta)*JacI[0][1] ) * JacI[1][0] +
      ( (*dxideta)*JacI[0][0] + (*deta2)  *JacI[0][1] ) * JacI[1][1]; 
     
  }
}


void elem_type_2D::JacobianSur_AD(const vector < vector < adept::adouble > > &vt, const unsigned &ig, adept::adouble &Weight, 
				  vector < double > &other_phi, vector < adept::adouble > &gradphi, vector < adept::adouble > &normal) const {
  other_phi.resize(nc_);
  normal.resize(3);
  
  adept::adouble Jac[3][3]={{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}};
 
  const double *dfx=dphidxi[ig];
  const double *dfy=dphideta[ig];
  
  for(int inode=0; inode<nc_; inode++,dfx++,dfy++){       
    Jac[0][0] += (*dfx)*vt[0][inode];
    Jac[1][0] += (*dfx)*vt[1][inode];
    Jac[2][0] += (*dfx)*vt[2][inode];
    
    Jac[0][1] += (*dfy)*vt[0][inode];
    Jac[1][1] += (*dfy)*vt[1][inode];
    Jac[2][1] += (*dfy)*vt[2][inode];
  }
    
    //   normal module
  adept::adouble nx = Jac[1][0]*Jac[2][1] - Jac[1][1]*Jac[2][0];
  adept::adouble ny = Jac[0][1]*Jac[2][0] - Jac[2][1]*Jac[0][0]; 
  adept::adouble nz = Jac[0][0]*Jac[1][1] - Jac[0][1]*Jac[1][0];
  adept::adouble invModn = 1./sqrt(nx*nx + ny*ny + nz*nz);

  normal[0] =  (nx)*invModn;
  normal[1] =  (ny)*invModn;
  normal[2] =  (nz)*invModn;
    
  Jac[0][2] = normal[0];
  Jac[1][2] = normal[1];
  Jac[2][2] = normal[2];
    
  //the determinant of the matrix is the area 
  adept::adouble det=(Jac[0][0]*(Jac[1][1]*Jac[2][2]-Jac[1][2]*Jac[2][1])+
		      Jac[0][1]*(Jac[1][2]*Jac[2][0]-Jac[1][0]*Jac[2][2])+
		      Jac[0][2]*(Jac[1][0]*Jac[2][1]-Jac[1][1]*Jac[2][0]));

  Weight=det*GaussWeight[ig];

  for(int inode=0;inode<nc_;inode++){
    other_phi[inode]=phi[ig][inode];
  }
  
  //TODO warning the surface gradient is missing!!!!!!!!!!!!!!!
}

void elem_type_2D::JacobianSur(const vector < vector < double > > &vt, const unsigned &ig, double &Weight, 
			       vector < double > &other_phi, vector < double > &gradphi, vector < double > &normal) const {
  other_phi.resize(nc_);
  normal.resize(3);
  
  double Jac[3][3]={{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}};
 
  const double *dfx=dphidxi[ig];
  const double *dfy=dphideta[ig];
  
  for(int inode=0; inode<nc_; inode++,dfx++,dfy++){       
    Jac[0][0] += (*dfx)*vt[0][inode];
    Jac[1][0] += (*dfx)*vt[1][inode];
    Jac[2][0] += (*dfx)*vt[2][inode];
    
    Jac[0][1] += (*dfy)*vt[0][inode];
    Jac[1][1] += (*dfy)*vt[1][inode];
    Jac[2][1] += (*dfy)*vt[2][inode];
  }
    
    //   normal module
  double nx = Jac[1][0]*Jac[2][1] - Jac[1][1]*Jac[2][0];
  double ny = Jac[0][1]*Jac[2][0] - Jac[2][1]*Jac[0][0]; 
  double nz = Jac[0][0]*Jac[1][1] - Jac[0][1]*Jac[1][0];
  double invModn = 1./sqrt(nx*nx + ny*ny + nz*nz);

  normal[0] =  (nx)*invModn;
  normal[1] =  (ny)*invModn;
  normal[2] =  (nz)*invModn;
    
  Jac[0][2] = normal[0];
  Jac[1][2] = normal[1];
  Jac[2][2] = normal[2];
    
  //the determinant of the matrix is the area 
  double det=(Jac[0][0]*(Jac[1][1]*Jac[2][2]-Jac[1][2]*Jac[2][1])+
		      Jac[0][1]*(Jac[1][2]*Jac[2][0]-Jac[1][0]*Jac[2][2])+
		      Jac[0][2]*(Jac[1][0]*Jac[2][1]-Jac[1][1]*Jac[2][0]));

  Weight=det*GaussWeight[ig];

  for(int inode=0;inode<nc_;inode++){
    other_phi[inode]=phi[ig][inode];
  }
  
  //TODO warning the surface gradient is missing!!!!!!!!!!!!!!!
}

//---------------------------------------------------------------------------------------------------------
void elem_type_3D::Jacobian(const vector < vector < double > > &vt,const unsigned &ig, double &Weight, 
			      vector < double > &other_phi, vector < double > &gradphi, vector < double > &nablaphi) const{
  
  other_phi.resize(nc_);
  gradphi.resize(nc_*3);
  nablaphi.resize(nc_*6);
				
				
  double Jac[3][3]={{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}};
  double JacI[3][3];
  
  const double *dxi=dphidxi[ig];
  const double *deta=dphideta[ig];
  const double *dzeta=dphidzeta[ig];

  for (int inode=0; inode<nc_; inode++,dxi++,deta++,dzeta++) {
    Jac[0][0]+=(*dxi)*vt[0][inode];
    Jac[0][1]+=(*dxi)*vt[1][inode];
    Jac[0][2]+=(*dxi)*vt[2][inode];
    Jac[1][0]+=(*deta)*vt[0][inode];
    Jac[1][1]+=(*deta)*vt[1][inode];
    Jac[1][2]+=(*deta)*vt[2][inode];
    Jac[2][0]+=(*dzeta)*vt[0][inode];
    Jac[2][1]+=(*dzeta)*vt[1][inode];
    Jac[2][2]+=(*dzeta)*vt[2][inode];
  } 
  double det=(Jac[0][0]*(Jac[1][1]*Jac[2][2]-Jac[1][2]*Jac[2][1])+
 	      Jac[0][1]*(Jac[1][2]*Jac[2][0]-Jac[1][0]*Jac[2][2])+
	      Jac[0][2]*(Jac[1][0]*Jac[2][1]-Jac[1][1]*Jac[2][0]));

  JacI[0][0]= (-Jac[1][2]*Jac[2][1] + Jac[1][1]*Jac[2][2])/det;
  JacI[0][1]= ( Jac[0][2]*Jac[2][1] - Jac[0][1]*Jac[2][2])/det;
  JacI[0][2]= (-Jac[0][2]*Jac[1][1] + Jac[0][1]*Jac[1][2])/det;
  JacI[1][0]= ( Jac[1][2]*Jac[2][0] - Jac[1][0]*Jac[2][2])/det;
  JacI[1][1]= (-Jac[0][2]*Jac[2][0] + Jac[0][0]*Jac[2][2])/det;
  JacI[1][2]= ( Jac[0][2]*Jac[1][0] - Jac[0][0]*Jac[1][2])/det;
  JacI[2][0]= (-Jac[1][1]*Jac[2][0] + Jac[1][0]*Jac[2][1])/det;
  JacI[2][1]= ( Jac[0][1]*Jac[2][0] - Jac[0][0]*Jac[2][1])/det;
  JacI[2][2]= (-Jac[0][1]*Jac[1][0] + Jac[0][0]*Jac[1][1])/det;

  Weight=det*GaussWeight[ig];
 
  dxi=dphidxi[ig];
  deta=dphideta[ig];
  dzeta=dphidzeta[ig];
  
  const double *dxi2=d2phidxi2[ig];
  const double *deta2=d2phideta2[ig];
  const double *dzeta2=d2phidzeta2[ig];
  const double *dxideta=d2phidxideta[ig];
  const double *detadzeta=d2phidetadzeta[ig];
  const double *dzetadxi=d2phidzetadxi[ig];
  
  for (int inode=0; inode<nc_; inode++, dxi++,deta++,dzeta++,dxi2++,deta2++,dzeta2++,dxideta++,detadzeta++,dzetadxi++) {
  
    other_phi[inode]=phi[ig][inode];
    
    gradphi[3*inode+0]=(*dxi)*JacI[0][0] + (*deta)*JacI[0][1] + (*dzeta)*JacI[0][2];
    gradphi[3*inode+1]=(*dxi)*JacI[1][0] + (*deta)*JacI[1][1] + (*dzeta)*JacI[1][2];
    gradphi[3*inode+2]=(*dxi)*JacI[2][0] + (*deta)*JacI[2][1] + (*dzeta)*JacI[2][2];
    
    nablaphi[6*inode+0]=
      ( (*dxi2)    *JacI[0][0] + (*dxideta)  *JacI[0][1] + (*dzetadxi) *JacI[0][2] )*JacI[0][0]+
      ( (*dxideta) *JacI[0][0] + (*deta2)    *JacI[0][1] + (*detadzeta)*JacI[0][2] )*JacI[0][1]+
      ( (*dzetadxi)*JacI[0][0] + (*detadzeta)*JacI[0][1] + (*dzeta2)   *JacI[0][2] )*JacI[0][2];
    nablaphi[6*inode+1]=
      ( (*dxi2)    *JacI[1][0] + (*dxideta)  *JacI[1][1] + (*dzetadxi) *JacI[1][2] )*JacI[1][0]+
      ( (*dxideta) *JacI[1][0] + (*deta2)    *JacI[1][1] + (*detadzeta)*JacI[1][2] )*JacI[1][1]+
      ( (*dzetadxi)*JacI[1][0] + (*detadzeta)*JacI[1][1] + (*dzeta2)   *JacI[1][2] )*JacI[1][2];
    nablaphi[6*inode+2]=
      ( (*dxi2)    *JacI[2][0] + (*dxideta)  *JacI[2][1] + (*dzetadxi) *JacI[2][2] )*JacI[2][0]+
      ( (*dxideta) *JacI[2][0] + (*deta2)    *JacI[2][1] + (*detadzeta)*JacI[2][2] )*JacI[2][1]+
      ( (*dzetadxi)*JacI[2][0] + (*detadzeta)*JacI[2][1] + (*dzeta2)   *JacI[2][2] )*JacI[2][2];
    nablaphi[6*inode+3]=
      ( (*dxi2)    *JacI[0][0] + (*dxideta)  *JacI[0][1] + (*dzetadxi) *JacI[0][2] )*JacI[1][0]+
      ( (*dxideta) *JacI[0][0] + (*deta2)    *JacI[0][1] + (*detadzeta)*JacI[0][2] )*JacI[1][1]+
      ( (*dzetadxi)*JacI[0][0] + (*detadzeta)*JacI[0][1] + (*dzeta2)   *JacI[0][2] )*JacI[1][2];
    nablaphi[6*inode+4]=
      ( (*dxi2)    *JacI[1][0] + (*dxideta)  *JacI[1][1] + (*dzetadxi) *JacI[1][2] )*JacI[2][0]+
      ( (*dxideta) *JacI[1][0] + (*deta2)    *JacI[1][1] + (*detadzeta)*JacI[1][2] )*JacI[2][1]+
      ( (*dzetadxi)*JacI[1][0] + (*detadzeta)*JacI[1][1] + (*dzeta2)   *JacI[1][2] )*JacI[2][2];
    nablaphi[6*inode+5]=
      ( (*dxi2)    *JacI[2][0] + (*dxideta)  *JacI[2][1] + (*dzetadxi) *JacI[2][2] )*JacI[0][0]+
      ( (*dxideta) *JacI[2][0] + (*deta2)    *JacI[2][1] + (*detadzeta)*JacI[2][2] )*JacI[0][1]+
      ( (*dzetadxi)*JacI[2][0] + (*detadzeta)*JacI[2][1] + (*dzeta2)   *JacI[2][2] )*JacI[0][2];
  }
  
}


void elem_type_3D::Jacobian_AD(const vector < vector < adept::adouble > > &vt,const unsigned &ig, adept::adouble &Weight, 
			      vector < double > &other_phi, vector < adept::adouble > &gradphi, vector < adept::adouble > &nablaphi) const{
  
  other_phi.resize(nc_);
  gradphi.resize(nc_*3);
  nablaphi.resize(nc_*6);
				
				
  adept::adouble Jac[3][3]={{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}};
  adept::adouble JacI[3][3];
  
  const double *dxi=dphidxi[ig];
  const double *deta=dphideta[ig];
  const double *dzeta=dphidzeta[ig];

  for (int inode=0; inode<nc_; inode++,dxi++,deta++,dzeta++) {
    Jac[0][0]+=(*dxi)*vt[0][inode];
    Jac[0][1]+=(*dxi)*vt[1][inode];
    Jac[0][2]+=(*dxi)*vt[2][inode];
    Jac[1][0]+=(*deta)*vt[0][inode];
    Jac[1][1]+=(*deta)*vt[1][inode];
    Jac[1][2]+=(*deta)*vt[2][inode];
    Jac[2][0]+=(*dzeta)*vt[0][inode];
    Jac[2][1]+=(*dzeta)*vt[1][inode];
    Jac[2][2]+=(*dzeta)*vt[2][inode];
  } 
  adept::adouble det=(Jac[0][0]*(Jac[1][1]*Jac[2][2]-Jac[1][2]*Jac[2][1])+
		      Jac[0][1]*(Jac[1][2]*Jac[2][0]-Jac[1][0]*Jac[2][2])+
		      Jac[0][2]*(Jac[1][0]*Jac[2][1]-Jac[1][1]*Jac[2][0]));

  JacI[0][0]= (-Jac[1][2]*Jac[2][1] + Jac[1][1]*Jac[2][2])/det;
  JacI[0][1]= ( Jac[0][2]*Jac[2][1] - Jac[0][1]*Jac[2][2])/det;
  JacI[0][2]= (-Jac[0][2]*Jac[1][1] + Jac[0][1]*Jac[1][2])/det;
  JacI[1][0]= ( Jac[1][2]*Jac[2][0] - Jac[1][0]*Jac[2][2])/det;
  JacI[1][1]= (-Jac[0][2]*Jac[2][0] + Jac[0][0]*Jac[2][2])/det;
  JacI[1][2]= ( Jac[0][2]*Jac[1][0] - Jac[0][0]*Jac[1][2])/det;
  JacI[2][0]= (-Jac[1][1]*Jac[2][0] + Jac[1][0]*Jac[2][1])/det;
  JacI[2][1]= ( Jac[0][1]*Jac[2][0] - Jac[0][0]*Jac[2][1])/det;
  JacI[2][2]= (-Jac[0][1]*Jac[1][0] + Jac[0][0]*Jac[1][1])/det;

  Weight=det*GaussWeight[ig];
 
  dxi=dphidxi[ig];
  deta=dphideta[ig];
  dzeta=dphidzeta[ig];
  
  const double *dxi2=d2phidxi2[ig];
  const double *deta2=d2phideta2[ig];
  const double *dzeta2=d2phidzeta2[ig];
  const double *dxideta=d2phidxideta[ig];
  const double *detadzeta=d2phidetadzeta[ig];
  const double *dzetadxi=d2phidzetadxi[ig];
  
  for (int inode=0; inode<nc_; inode++, dxi++,deta++,dzeta++,dxi2++,deta2++,dzeta2++,dxideta++,detadzeta++,dzetadxi++) {
  
    other_phi[inode]=phi[ig][inode];
    
    gradphi[3*inode+0]=(*dxi)*JacI[0][0] + (*deta)*JacI[0][1] + (*dzeta)*JacI[0][2];
    gradphi[3*inode+1]=(*dxi)*JacI[1][0] + (*deta)*JacI[1][1] + (*dzeta)*JacI[1][2];
    gradphi[3*inode+2]=(*dxi)*JacI[2][0] + (*deta)*JacI[2][1] + (*dzeta)*JacI[2][2];
    
    nablaphi[6*inode+0]=
      ( (*dxi2)    *JacI[0][0] + (*dxideta)  *JacI[0][1] + (*dzetadxi) *JacI[0][2] )*JacI[0][0]+
      ( (*dxideta) *JacI[0][0] + (*deta2)    *JacI[0][1] + (*detadzeta)*JacI[0][2] )*JacI[0][1]+
      ( (*dzetadxi)*JacI[0][0] + (*detadzeta)*JacI[0][1] + (*dzeta2)   *JacI[0][2] )*JacI[0][2];
    nablaphi[6*inode+1]=
      ( (*dxi2)    *JacI[1][0] + (*dxideta)  *JacI[1][1] + (*dzetadxi) *JacI[1][2] )*JacI[1][0]+
      ( (*dxideta) *JacI[1][0] + (*deta2)    *JacI[1][1] + (*detadzeta)*JacI[1][2] )*JacI[1][1]+
      ( (*dzetadxi)*JacI[1][0] + (*detadzeta)*JacI[1][1] + (*dzeta2)   *JacI[1][2] )*JacI[1][2];
    nablaphi[6*inode+2]=
      ( (*dxi2)    *JacI[2][0] + (*dxideta)  *JacI[2][1] + (*dzetadxi) *JacI[2][2] )*JacI[2][0]+
      ( (*dxideta) *JacI[2][0] + (*deta2)    *JacI[2][1] + (*detadzeta)*JacI[2][2] )*JacI[2][1]+
      ( (*dzetadxi)*JacI[2][0] + (*detadzeta)*JacI[2][1] + (*dzeta2)   *JacI[2][2] )*JacI[2][2];
    nablaphi[6*inode+3]=
      ( (*dxi2)    *JacI[0][0] + (*dxideta)  *JacI[0][1] + (*dzetadxi) *JacI[0][2] )*JacI[1][0]+
      ( (*dxideta) *JacI[0][0] + (*deta2)    *JacI[0][1] + (*detadzeta)*JacI[0][2] )*JacI[1][1]+
      ( (*dzetadxi)*JacI[0][0] + (*detadzeta)*JacI[0][1] + (*dzeta2)   *JacI[0][2] )*JacI[1][2];
    nablaphi[6*inode+4]=
      ( (*dxi2)    *JacI[1][0] + (*dxideta)  *JacI[1][1] + (*dzetadxi) *JacI[1][2] )*JacI[2][0]+
      ( (*dxideta) *JacI[1][0] + (*deta2)    *JacI[1][1] + (*detadzeta)*JacI[1][2] )*JacI[2][1]+
      ( (*dzetadxi)*JacI[1][0] + (*detadzeta)*JacI[1][1] + (*dzeta2)   *JacI[1][2] )*JacI[2][2];
    nablaphi[6*inode+5]=
      ( (*dxi2)    *JacI[2][0] + (*dxideta)  *JacI[2][1] + (*dzetadxi) *JacI[2][2] )*JacI[0][0]+
      ( (*dxideta) *JacI[2][0] + (*deta2)    *JacI[2][1] + (*detadzeta)*JacI[2][2] )*JacI[0][1]+
      ( (*dzetadxi)*JacI[2][0] + (*detadzeta)*JacI[2][1] + (*dzeta2)   *JacI[2][2] )*JacI[0][2];
  }
  
}

} //end namespace femus


