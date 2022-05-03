void AssembleGhostPenaltyP(MultiLevelProblem& ml_prob, const bool &omega1) {

  clock_t start_time;

  //pointers and references

  LinearImplicitSystem& mlPdeSys  = ml_prob.get_system<LinearImplicitSystem> ("Nitsche");   // pointer to the linear implicit system named "Poisson"
  const unsigned  level = mlPdeSys.GetLevelToAssemble();
  MultiLevelSolution* mlSol = ml_prob._ml_sol;  // pointer to the multilevel solution object
  Solution* mysolution = mlSol->GetSolutionLevel(level);     // pointer to the solution (level) object

  //Solution*                sol = ml_prob._ml_sol->GetSolutionLevel(level);    // pointer to the solution (level) object

  LinearEquationSolver* myLinEqSolver = mlPdeSys._LinSolver[level];  // pointer to the equation (level) object

  Mesh* msh = ml_prob._ml_msh->GetLevel(level);     // pointer to the mesh (level) object
  elem* el = msh->el;   // pointer to the elem object in msh (level)
  SparseMatrix* myKK = myLinEqSolver->_KK;  // pointer to the global stifness matrix object in pdeSys (level)
  NumericVector* myRES =  myLinEqSolver->_RES;  // pointer to the global residual vector object in pdeSys (level)

  MatSetOption((static_cast< PetscMatrix* >(myKK))->mat(), MAT_NEW_NONZERO_ALLOCATION_ERR, PETSC_FALSE);


  // call the adept stack object
  adept::Stack& s = FemusInit::_adeptStack;

  const unsigned dim = msh->GetDimension();
  const unsigned dim2 = 3 * (dim - 1);

  // data
  unsigned iproc  = msh->processor_id();
  unsigned nprocs  = msh->n_processors();

  //quantities for iel will have index1
  //quantities for jel will have index2
  
  //**** is this solution of omega1 and omega2? or is it solution of one element and neibouring element?
  vector< adept::adouble > solu1; // local solution (velocity)//***is it just a vector?
  vector< adept::adouble > solu2; // local solution (velocity)

  vector< adept::adouble > aRes1;     // local redidual vector//*****is it a vector of 1?
  vector< adept::adouble > aRes2;     // local redidual vector

  vector< double > rhs1; // local redidual vector
  vector< double > rhs2; // local redidual vector
  vector < double > Jac; 

  std::vector <unsigned> sysDofs1;//*****is it a vector of 1? No
  std::vector <unsigned> sysDofs2;

  double weight;
  std::vector < double > phi;
  std::vector < double> gradPhi;

  //*** referes to solution of the element? is it also for neibouringelements?
  double weight1;
  std::vector < double > phi1;
  std::vector < double> gradPhi1;
  std::vector < double> nablaPhi1;

  double weight2;
  std::vector < double > phi2;
  std::vector < double> gradPhi2;
  std::vector < double> nablaPhi2;

  //****coordinates of two neibouring elements?
  vector <vector < double> > vx1(dim);
  vector <vector < double> > vx2(dim);

  //reading parameters for fluid FEM domain
  double alpha = (omega1) ? alpha1 : alpha2;


  std::cout.precision(10);

  //variable-name handling
  const char varname[2][3] = {"u1", "u2"};//vector of strings. two entries and atmost 3 characters
  
  //***if omega1 is true, it's going to give you u1. if it is false, it is u2.
  unsigned indexSol = mlSol->GetIndex(&varname[1 - omega1][0]);
  unsigned indexPde = mlPdeSys.GetSolPdeIndex(&varname[1 - omega1][0]);
  unsigned solType = mlSol->GetSolutionType(&varname[0][0]);

  unsigned eflagIndex = mlSol->GetIndex("eflag");//**will tell you whether we have a cut or not. if eflag=1 we have a cut cell.
  
  start_time = clock();

  std::vector < std::vector < std::vector <double > > > aP1(3);
  std::vector < std::vector < std::vector <double > > > aP2(3);


  //flagmark
  //**here we are looping on the element on the mesh
  for(int iel = msh->_elementOffset[iproc]; iel < msh->_elementOffset[iproc + 1]; iel++) {
    //***if we have a cut element go in
    unsigned eFlag1 = static_cast <unsigned>(floor((*mysolution->_Sol[eflagIndex])(iel) + 0.25));
    if(eFlag1 == 1) {
      //**if it is a cut element we need to know what type of cut element we have and number of nodes we have. 
      short unsigned ielt1 = msh->GetElementType(iel);
      unsigned nDofs1 = msh->GetElementDofNumber(iel, solType);    // number of solution element dofs
      // resize local arrays
      sysDofs1.resize(nDofs1);
      solu1.resize(nDofs1);
      for(unsigned  k = 0; k < dim; k++) {
        vx1[k].resize(nDofs1);
      }

      for(unsigned i = 0; i < nDofs1; i++) {
        unsigned idof = msh->GetSolutionDof(i, iel, solType);
        solu1[i] = (*mysolution->_Sol[indexSol])(idof);
        sysDofs1[i] = myLinEqSolver->GetSystemDof(indexSol, indexPde, i, iel);
      }

      for(unsigned i = 0; i < nDofs1; i++) {
        unsigned idofX = msh->GetSolutionDof(i, iel, 2);
        for(unsigned  k = 0; k < dim; k++) {
          vx1[k][i] = (*msh->_topology->_Sol[k])(idofX);
        }
      }

      bool aP1IsInitialized = false;
      //**looping on faces of element iel
      for(unsigned iface = 0; iface < msh->GetElementFaceNumber(iel); iface++) {
        int jel = el->GetFaceElementIndex(iel, iface) - 1;//**jel is the neibouring element
        if(jel >= 0) { // iface is not a boundary of the domain.
        //**if we are on the boundary jel is negative number.we ignore that case
          unsigned jproc = msh->IsdomBisectionSearch(jel, 3);
          if(jproc == iproc) {//**iel and jel belongs to the same processor
            //**checking flag of jel
            unsigned eFlag2 = static_cast <unsigned>(floor((*mysolution->_Sol[eflagIndex])(jel) + 0.25));
            //**if you are a interior cell of same type or if it is a cut element with jel>iel (as we want to do integral one type)
            if(eFlag2 == 0 + !omega1 * 2 || (eFlag2 == 1 && jel > iel)) {

              short unsigned ielt2 = msh->GetElementType(jel);
              unsigned nDofs2 = msh->GetElementDofNumber(jel, solType);    // number of solution element dofs
              // resize local arrays
              sysDofs2.resize(nDofs2);
              solu2.resize(nDofs2);

              for(unsigned  k = 0; k < dim; k++) {
                vx2[k].resize(nDofs2);
              }
              for(unsigned i = 0; i < nDofs2; i++) {
                unsigned idof = msh->GetSolutionDof(i, jel, solType);
                solu2[i] = (*mysolution->_Sol[indexSol])(idof);
                sysDofs2[i] = myLinEqSolver->GetSystemDof(indexSol, indexPde, i, jel);
              }
              for(unsigned i = 0; i < nDofs2; i++) {
                unsigned idofX = msh->GetSolutionDof(i, jel, 2);
                for(unsigned  k = 0; k < dim; k++) {
                  vx2[k][i] = (*msh->_topology->_Sol[k])(idofX);
                }
              }
              //**now we are on iel and on a particular face and corresponding jel. we have all the information. 
              aRes1.assign(nDofs1, 0.);
              aRes2.assign(nDofs2, 0.);

              s.new_recording();
              //**gathering information on the boundary 
              const unsigned faceGeom = msh->GetElementFaceType(iel, iface);
              unsigned faceDofs = msh->GetElementFaceDofNumber(iel, iface, solType);
              std::vector  < std::vector  <  double > > faceVx(dim);    // A matrix holding the face coordinates rowwise.
              for(int k = 0; k < dim; k++) {
                faceVx[k].resize(faceDofs);
              }
              for(unsigned i = 0; i < faceDofs; i++) {
                unsigned inode = msh->GetLocalFaceVertexIndex(iel, iface, i);    // face-to-element local node mapping.
                for(unsigned k = 0; k < dim; k++) {
                  faceVx[k][i] =  vx1[k][inode]; // We extract the local coordinates on the face from local coordinates on the element.
                }
              }

              double h = sqrt(dim - 1) * sqrt((faceVx[0][0] - faceVx[0][1]) * (faceVx[0][0] - faceVx[0][1]) +
                                              (faceVx[1][0] - faceVx[1][1]) * (faceVx[1][0] - faceVx[1][1])) ;
              double h2 = h * h;
              double h3 = h * h * h;


              if(!aP1IsInitialized) { //build the basis 1,x,y,z... corresponding to the solution type
                aP1IsInitialized = true;
                for(unsigned jtype = 0; jtype < solType + 1; jtype++) {
                  ProjectNodalToPolynomialCoefficients(aP1[jtype], vx1, ielt1, jtype);
                }
              }

              for(unsigned jtype = 0; jtype < solType + 1; jtype++) {
                ProjectNodalToPolynomialCoefficients(aP2[jtype], vx2, ielt2, jtype);
              }
              //**gauss loop on the face
              for(unsigned ig = 0; ig  <  msh->_finiteElement[faceGeom][solType]->GetGaussPointNumber(); ig++) {

                std::vector < double> normal;//**normal is going from iel to jel
                msh->_finiteElement[faceGeom][solType]->JacobianSur(faceVx, ig, weight, phi, gradPhi, normal);//**here gradient for the test function is completely fake. important phi on the edge. we use phi to get physical gauss coordinates

                std::vector< double > xg(dim, 0.); // physical coordinates of the face Gauss point
                for(unsigned i = 0; i < faceDofs; i++) {
                  for(unsigned k = 0; k < dim; k++) {
                    xg[k] += phi[i] * faceVx[k][i];
                  }
                }
                //**now we can do the inverse mapping 
                std::vector <double> xi1;//local coordinates of the face gauss point with respect to iel
                GetClosestPointInReferenceElement(vx1, xg, ielt1, xi1);

                bool inverseMapping = GetInverseMapping(solType, ielt1, aP1, xg, xi1, 100);
                if(!inverseMapping) {
                  std::cout << "InverseMapping1 failed at " << iel << " " << jel << " " << iface << std::endl;
                }

                std::vector <double> xi2;//local coordinates of the face gauss point with respect to jel
                GetClosestPointInReferenceElement(vx2, xg, ielt2, xi2);

                inverseMapping = GetInverseMapping(solType, ielt2, aP2, xg, xi2, 100);
                if(!inverseMapping) {
                  std::cout << "InverseMapping2 failed at " << iel << " " << jel << " " << iface << std::endl;
                }
                
                //** having xi1 and xi2, now we can get gradient of laplace and the test function
                msh->_finiteElement[ielt1][solType]->Jacobian(vx1, xi1, weight1, phi1, gradPhi1, nablaPhi1);
                msh->_finiteElement[ielt2][solType]->Jacobian(vx2, xi2, weight2, phi2, gradPhi2, nablaPhi2);
                
                //**now you have surface are and normal. now we can assemble penalty term. 

                adept::adouble gradSolu1DotN = 0.;//**gradient of the solution one in normal direction
                adept::adouble hessSolu1DotN = 0.;//** Hessian of the solution in the normal direction
                for(unsigned i = 0; i < nDofs1; i++) {
                  for(unsigned J = 0; J < dim; J++) {
                      
                      //**dot product between the gradient and the normal
                      //**gradient of phi in the direction j multiply by normal j
                    gradSolu1DotN += solu1[i] * gradPhi1[i * dim + J] * normal[J];
                    for(unsigned K = 0; K < dim; K++) {
                      //2D xx, yy, xy
                      //3D xx, yy, zz, xy, yz ,zx
                      unsigned L;
                      if(J == K) L = J;
                      else if(1 == J + K) L = dim;     // xy
                      else if(2 == J + K) L = dim + 2; // xz
                      else if(3 == J + K) L = dim + 1; // yz
                      hessSolu1DotN += solu1[i] * normal[J] * nablaPhi1[i * dim2 + L] * normal[K];
                    }
                  }
                }
//** same thing should be done for element 2
                adept::adouble gradSolu2DotN = 0.;
                adept::adouble hessSolu2DotN = 0.;
                for(unsigned i = 0; i < nDofs2; i++) {
                  for(unsigned J = 0; J < dim; J++) {
                    gradSolu2DotN += solu2[i] * gradPhi2[i * dim + J] * normal[J];
                    for(unsigned K = 0; K < dim; K++) {
                      //2D xx, yy, xy
                      //3D xx, yy, zz, xy, yz ,zx
                      unsigned L;
                      if(J == K) L = J;
                      else if(1 == J + K) L = dim;     // xy
                      else if(2 == J + K) L = dim + 2; // xz
                      else if(3 == J + K) L = dim + 1; // yz
                      hessSolu2DotN += solu2[i] * normal[J] * nablaPhi2[i * dim2 + L] * normal[K];
                    }
                  }
                }


                double C1 = 0.05 * alpha;  // [alpha];

                for(unsigned i = 0; i < nDofs1; i++) { 

                  for(unsigned J = 0; J < dim; J++) {
                    aRes1[i] +=  C1 * h * gradPhi1[i * dim + J] * normal[J] * (gradSolu1DotN - gradSolu2DotN) * weight;



                    for(unsigned K = 0; K < dim; K++) {

                      unsigned L;
                      if(J == K) L = J;
                      else if(1 == J + K) L = dim;     // xy
                      else if(2 == J + K) L = dim + 2; // xz
                      else if(3 == J + K) L = dim + 1; // yz

                      aRes1[i] += C1 * h3 * normal[J] * nablaPhi1[i * dim2 + L] * normal[K] * (hessSolu1DotN - hessSolu2DotN) * weight;
                    }
                  }
                }

                for(unsigned i = 0; i < nDofs2; i++) {

                  for(unsigned J = 0; J < dim; J++) {
                    aRes2[i] +=  -C1 * h * gradPhi2[i * dim + J] * normal[J] * (gradSolu1DotN - gradSolu2DotN) * weight;

                    for(unsigned K = 0; K < dim; K++) {

                      unsigned L;
                      if(J == K) L = J;
                      else if(1 == J + K) L = dim;     // xy
                      else if(2 == J + K) L = dim + 2; // xz
                      else if(3 == J + K) L = dim + 1; // yz

                      aRes2[i] +=  -C1 * h3 * normal[J] * nablaPhi2[i * dim2 + L] * normal[K] * (hessSolu1DotN - hessSolu2DotN) * weight;
                    }
                  }
                }


              }

              //copy the value of the adept::adoube aRes in double Res and store them in RES
              rhs1.resize(nDofs1);   //resize
              for(int i = 0; i < nDofs1; i++) {
                rhs1[ i ] = -aRes1[i].value();
              }
              myRES->add_vector_blocked(rhs1, sysDofs1);


              rhs2.resize(nDofs2);   //resize
              for(int i = 0; i < nDofs2; i++) {
                rhs2[  i ] = -aRes2[i].value();
              }
              myRES->add_vector_blocked(rhs2, sysDofs2);


              // define the dependent and independent variables J11
              s.dependent(&aRes1[0], nDofs1);
              s.independent(&solu1[0], nDofs1);

              Jac.resize(nDofs1 * nDofs1);
              s.jacobian(&Jac[0], true);
              myKK->add_matrix_blocked(Jac, sysDofs1, sysDofs1);

              s.clear_independents();
              s.independent(&solu2[0], nDofs2);

              Jac.resize(nDofs1 * nDofs2);
              s.jacobian(&Jac[0], true);
              myKK->add_matrix_blocked(Jac, sysDofs1, sysDofs2);

              s.clear_independents();
              s.clear_dependents(); // for J11 and J12


              // define the dependent and independent variables J21
              s.dependent(&aRes2[0], nDofs2);
              s.independent(&solu1[0], nDofs1);

              Jac.resize(nDofs2 * nDofs1);
              // get the and store jacobian matrix (row-major)
              s.jacobian(&Jac[0], true);
              myKK->add_matrix_blocked(Jac, sysDofs2, sysDofs1);
              s.clear_independents();

              s.independent(&solu2[0], nDofs2);
              Jac.resize(nDofs2 * nDofs2);
              // get the and store jacobian matrix (row-major)
              s.jacobian(&Jac[0], true);
              myKK->add_matrix_blocked(Jac, sysDofs2, sysDofs2);
              s.clear_independents();

              s.clear_dependents(); // for J21 and J22
            }
          }
        }
      }
    }
  }

//   //flagmark
//   if(nprocs > 1) {
//     for(unsigned kproc = 0; kproc < nprocs; kproc++) {
//       for(int iel = msh->_elementOffset[kproc]; iel < msh->_elementOffset[kproc + 1]; iel++) {
// 
//         unsigned eFlag1;
// 
//         if(iproc == kproc) {
//           eFlag1 = static_cast <unsigned>(floor((*mysolution->_Sol[eflagIndex])(iel) + 0.25));
//         }
//         MPI_Bcast(&eFlag1, 1, MPI_UNSIGNED, kproc, PETSC_COMM_WORLD);
// 
//         if(eFlag1 == 1) {
//           unsigned nFaces;
//           if(iproc == kproc) {
//             nFaces = msh->GetElementFaceNumber(iel);
//           }
//           MPI_Bcast(&nFaces, 1, MPI_UNSIGNED, kproc, PETSC_COMM_WORLD);
// 
//           for(unsigned iface = 0; iface < nFaces; iface++) {
// 
//             int jel;
//             if(iproc == kproc) {
//               jel = el->GetFaceElementIndex(iel, iface) - 1;
//             }
//             MPI_Bcast(&jel, 1, MPI_INT, kproc, PETSC_COMM_WORLD);
// 
//             if(jel >= 0) { // iface is not a boundary of the domain
//               unsigned jproc = msh->IsdomBisectionSearch(jel, 3);  // return  jproc for piece-wise constant discontinuous type (3)
//               if(jproc != kproc && (iproc == kproc || iproc == jproc)) {
// 
//                 unsigned eFlag2;
//                 if(iproc == jproc) {
//                   eFlag2 = static_cast <unsigned>(floor((*mysolution->_Sol[eflagIndex])(jel) + 0.25));
//                   MPI_Send(&eFlag2, 1, MPI_UNSIGNED, kproc, 0, PETSC_COMM_WORLD);
//                 }
//                 else if(iproc == kproc) {
//                   MPI_Recv(&eFlag2, 1, MPI_UNSIGNED, jproc, 0, PETSC_COMM_WORLD, MPI_STATUS_IGNORE);
//                 }
// 
//                 if(eFlag2 == 0 + !fluid * 2 || (eFlag2 == 1 && jel > iel)) {
//                   //std::cout << "I am " << iel << " on " << kproc << " talking with " << jel << " on " << jproc << std::endl;
// 
//                   short unsigned ielt1;
//                   short unsigned ielt2;
// 
//                   if(iproc == kproc) {
//                     ielt1 = msh->GetElementType(iel);
//                     MPI_Recv(&ielt2, 1, MPI_UNSIGNED_SHORT, jproc, 0, PETSC_COMM_WORLD, MPI_STATUS_IGNORE);
//                   }
//                   else if(iproc == jproc) {
//                     ielt2 = msh->GetElementType(jel);
//                     MPI_Send(&ielt2, 1, MPI_UNSIGNED_SHORT, kproc, 0, PETSC_COMM_WORLD);
//                   }
// 
//                   unsigned nDofs1;
//                   unsigned nDofsP1;
//                   unsigned nDofsAll1;
// 
//                   unsigned nDofs2 = el->GetNVE(ielt2, solType);
//                   unsigned nDofsP2 = el->GetNVE(ielt2, solTypeP);
//                   unsigned nDofsAll2 = dim * nDofs2 + nDofsP2;
// 
//                   sysDofs2.resize(nDofsAll2);
//                   for(unsigned  k = 0; k < dim; k++) {
//                     sol2d[k].resize(nDofs2);
//                     sol2Old[k].resize(nDofs2);
//                     vx2[k].resize(nDofs2);
//                   }
//                   solP2d.resize(nDofsP2);
//                   std::vector < MPI_Request > reqs(3 * dim + 2);
//                   if(iproc == kproc) {
// 
//                     nDofs1 = el->GetNVE(ielt1, solType);
//                     nDofsP1 = el->GetNVE(ielt1, solTypeP);
//                     nDofsAll1 = dim * nDofs1 + nDofsP1;
// 
//                     sysDofs1.resize(nDofsAll1);
// 
//                     for(unsigned  k = 0; k < dim; k++) {
//                       sol1[k].resize(nDofs1);
//                       sol1Old[k].resize(nDofs1);
//                       vx1[k].resize(nDofs1);
//                       if(fluid) solDTld[k].resize(nDofs1);
//                     }
//                     solP1.resize(nDofsP1);
// 
//                     for(unsigned i = 0; i < nDofs1; i++) {
//                       unsigned idof = msh->GetSolutionDof(i, iel, solType);
// 
//                       for(unsigned  k = 0; k < dim; k++) {
//                         sol1[k][i] = (*mysolution->_Sol[indexSol[k]])(idof);
//                         sol1Old[k][i] = (*mysolution->_SolOld[indexSol[k]])(idof);
//                         if(fluid) solDTld[k][i] = (*mysolution->_Sol[indexSolD[k]])(idof) - (*mysolution->_SolOld[indexSolD[k]])(idof);
//                         sysDofs1[k * nDofs1 + i] = myLinEqSolver->GetSystemDof(indexSol[k], indexPde[k], i, iel);
//                       }
//                     }
// 
//                     for(unsigned i = 0; i < nDofsP1; i++) {
//                       unsigned idof = msh->GetSolutionDof(i, iel, solTypeP);
//                       solP1[i] = (*mysolution->_Sol[indexSolP])(idof);
//                       sysDofs1[dim * nDofs1 + i] = myLinEqSolver->GetSystemDof(indexSolP, indexPdeP, i, iel);
//                     }
// 
//                     for(unsigned i = 0; i < nDofs1; i++) {
//                       unsigned idofX = msh->GetSolutionDof(i, iel, 2);
//                       for(unsigned  k = 0; k < dim; k++) {
//                         vx1[k][i] = (*msh->_topology->_Sol[k])(idofX);
//                       }
//                     }
//                     for(unsigned k = 0; k < dim; k++) {
//                       MPI_Irecv(sol2d[k].data(), sol2d[k].size(), MPI_DOUBLE, jproc, k, PETSC_COMM_WORLD, &reqs[k]);
//                       MPI_Irecv(sol2Old[k].data(), sol2Old[k].size(), MPI_DOUBLE, jproc, k + dim, PETSC_COMM_WORLD, &reqs[k + dim]);
//                       MPI_Irecv(vx2[k].data(), vx2[k].size(), MPI_DOUBLE, jproc, k + 2 * dim, PETSC_COMM_WORLD, &reqs[k + 2 * dim]);
//                     }
//                     MPI_Irecv(solP2d.data(), solP2d.size(), MPI_DOUBLE, jproc, 3 * dim, PETSC_COMM_WORLD,  &reqs[3 * dim]);
//                     MPI_Irecv(sysDofs2.data(), sysDofs2.size(), MPI_UNSIGNED, jproc, 3 * dim + 1, PETSC_COMM_WORLD,  &reqs[3 * dim + 1]);
//                   }
//                   else if(iproc == jproc) {
//                     for(unsigned i = 0; i < nDofs2; i++) {
//                       unsigned idof = msh->GetSolutionDof(i, jel, solType);
// 
//                       for(unsigned  k = 0; k < dim; k++) {
//                         sol2d[k][i] = (*mysolution->_Sol[indexSol[k]])(idof);
//                         sol2Old[k][i] = (*mysolution->_SolOld[indexSol[k]])(idof);
//                         sysDofs2[k * nDofs2 + i] = myLinEqSolver->GetSystemDof(indexSol[k], indexPde[k], i, jel);
//                       }
//                     }
// 
//                     for(unsigned i = 0; i < nDofsP2; i++) {
//                       unsigned idof = msh->GetSolutionDof(i, jel, solTypeP);
//                       solP2d[i] = (*mysolution->_Sol[indexSolP])(idof);
//                       sysDofs2[dim * nDofs2 + i] = myLinEqSolver->GetSystemDof(indexSolP, indexPdeP, i, jel);
//                     }
// 
//                     for(unsigned i = 0; i < nDofs2; i++) {
//                       unsigned idofX = msh->GetSolutionDof(i, jel, 2);
//                       for(unsigned  k = 0; k < dim; k++) {
//                         vx2[k][i] = (*msh->_topology->_Sol[k])(idofX);
//                       }
//                     }
// 
//                     for(unsigned k = 0; k < dim; k++) {
//                       MPI_Isend(sol2d[k].data(), sol2d[k].size(), MPI_DOUBLE, kproc, k, PETSC_COMM_WORLD, &reqs[k]);
//                       MPI_Isend(sol2Old[k].data(), sol2Old[k].size(), MPI_DOUBLE, kproc, k + dim, PETSC_COMM_WORLD, &reqs[k + dim]);
//                       MPI_Isend(vx2[k].data(), vx2[k].size(), MPI_DOUBLE, kproc, k + 2 * dim, PETSC_COMM_WORLD, &reqs[k + 2 * dim]);
//                     }
//                     MPI_Isend(solP2d.data(), solP2d.size(), MPI_DOUBLE, kproc, 3 * dim, PETSC_COMM_WORLD, &reqs[3 * dim]);
//                     MPI_Isend(sysDofs2.data(), sysDofs2.size(), MPI_UNSIGNED, kproc, 3 * dim + 1, PETSC_COMM_WORLD, &reqs[3 * dim + 1]);
//                   }
// 
//                   MPI_Status status;
//                   for(unsigned m = 0; m < 3 * dim + 2; m++) {
//                     MPI_Wait(&reqs[m], &status);
//                   }
// 
//                   if(iproc == kproc) {
// 
//                     for(unsigned  k = 0; k < dim; k++) {
//                       sol2[k].resize(nDofs2);
//                       for(unsigned i = 0; i < nDofs2; i++) {
//                         sol2[k][i] = sol2d[k][i];
//                       }
//                     }
//                     solP2.resize(nDofsP2);
//                     for(unsigned i = 0; i < nDofsP2; i++) {
//                       solP2[i] = solP2d[i];
//                     }
// 
// 
//                     for(unsigned  k = 0; k < dim; k++) {
//                       aRes1[k].assign(nDofs1, 0.);
//                       aRes2[k].assign(nDofs2, 0.);
//                     }
//                     aResP1.assign(nDofsP1, 0.);
//                     aResP2.assign(nDofsP2, 0.);
// 
//                     s.new_recording();
// 
//                     const unsigned faceGeom = el->GetFaceType(ielt1, iface);
//                     unsigned faceDofs = el->GetNFACENODES(ielt1, iface, solType);
// 
//                     std::vector  < std::vector  <  double > > faceVx(dim);    // A matrix holding the face coordinates rowwise.
//                     for(int k = 0; k < dim; k++) {
//                       faceVx[k].resize(faceDofs);
//                     }
//                     for(unsigned i = 0; i < faceDofs; i++) {
//                       unsigned inode = msh->GetLocalFaceVertexIndex(iel, iface, i);    // face-to-element local node mapping.
//                       for(unsigned k = 0; k < dim; k++) {
//                         faceVx[k][i] =  vx1[k][inode]; // We extract the local coordinates on the face from local coordinates on the element.
//                       }
//                     }
// 
//                     double h = sqrt(dim - 1) * sqrt((faceVx[0][0] - faceVx[0][1]) * (faceVx[0][0] - faceVx[0][1]) +
//                                                     (faceVx[1][0] - faceVx[1][1]) * (faceVx[1][0] - faceVx[1][1])) ;
//                     double h2 = h * h;
//                     double h3 = h * h * h;
// 
//                     //std::cout << " h = " << h << " ";
// 
//                     for(unsigned jtype = 0; jtype < solType + 1; jtype++) {
//                       ProjectNodalToPolynomialCoefficients(aP1[jtype], vx1, ielt1, jtype);
//                       ProjectNodalToPolynomialCoefficients(aP2[jtype], vx2, ielt2, jtype);
//                     }
// 
//                     for(unsigned ig = 0; ig  <  msh->_finiteElement[faceGeom][solType]->GetGaussPointNumber(); ig++) {
// 
//                       std::vector < double> normal;
//                       msh->_finiteElement[faceGeom][solType]->JacobianSur(faceVx, ig, weight, phi, gradPhi, normal);
// 
//                       std::vector< double > xg(dim, 0.); // physical coordinates of the face Gauss point
//                       for(unsigned i = 0; i < faceDofs; i++) {
//                         for(unsigned k = 0; k < dim; k++) {
//                           xg[k] += phi[i] * faceVx[k][i];
//                         }
//                       }
// 
//                       std::vector <double> xi1;//local coordinates of the face gauss point with respect to iel
//                       GetClosestPointInReferenceElement(vx1, xg, ielt1, xi1);
// 
//                       bool inverseMapping = GetInverseMapping(solType, ielt1, aP1, xg, xi1, 100);
//                       if(!inverseMapping) {
//                         std::cout << "InverseMapping1 failed at " << iel << " " << jel << " " << iface << std::endl;
//                       }
// 
//                       std::vector <double> xi2;//local coordinates of the face gauss point with respect to jel
//                       GetClosestPointInReferenceElement(vx2, xg, ielt2, xi2);
// 
//                       inverseMapping = GetInverseMapping(solType, ielt2, aP2, xg, xi2, 100);
//                       if(!inverseMapping) {
//                         std::cout << "InverseMapping2 failed at " << iel << " " << jel << " " << iface << std::endl;
//                       }
// 
//                       msh->_finiteElement[ielt1][solType]->Jacobian(vx1, xi1, weight1, phi1, gradPhi1, nablaPhi1);
//                       msh->_finiteElement[ielt2][solType]->Jacobian(vx2, xi2, weight2, phi2, gradPhi2, nablaPhi2);
// 
//                       std::vector < double > aleVel(dim, 0.);
// 
//                       adept::adouble divSol1g = 0.;
//                       adept::adouble divSol2g = 0.;
// 
//                       std::vector < adept::adouble > gradSol1DotN(dim, 0.);
//                       std::vector < adept::adouble > gradSol2DotN(dim, 0.);
// 
//                       std::vector < adept::adouble >  hessSol1DotN(dim, 0.);
//                       std::vector < adept::adouble >  hessSol2DotN(dim, 0.);
// 
//                       for(unsigned I = 0; I < dim; I++) {
//                         for(unsigned i = 0; i < nDofs1; i++) {
//                           divSol1g = sol1[I][i] * gradPhi1[i * dim + I];
//                           if(fluid) aleVel[I] += phi1[i] * (sol1Old[I][i] - solDTld[I][i] / dt);
//                           for(unsigned J = 0; J < dim; J++) {
//                             gradSol1DotN[I] += sol1[I][i] * gradPhi1[i * dim + J] * normal[J];
//                             for(unsigned K = 0; K < dim; K++) {
//                               //2D xx, yy, xy
//                               //3D xx, yy, zz, xy, yz ,zx
//                               unsigned L;
//                               if(J == K) L = J;
//                               else if(1 == J + K) L = dim;     // xy
//                               else if(2 == J + K) L = dim + 2; // xz
//                               else if(3 == J + K) L = dim + 1; // yz
//                               hessSol1DotN[I] += sol1[I][i] * normal[J] * nablaPhi1[i * dim2 + L] * normal[K];
//                             }
//                           }
//                         }
//                       }
// 
//                       for(unsigned I = 0; I < dim; I++) {
//                         for(unsigned i = 0; i < nDofs2; i++) {
//                           divSol2g = sol2[I][i] * gradPhi2[i * dim + I];
//                           for(unsigned J = 0; J < dim; J++) {
//                             gradSol2DotN[I] += sol2[I][i] * gradPhi2[i * dim + J] * normal[J];
//                             for(unsigned K = 0; K < dim; K++) {
//                               //2D xx, yy, xy
//                               //3D xx, yy, zz, xy, yz ,zx
//                               unsigned L;
//                               if(J == K) L = J;
//                               else if(1 == J + K) L = dim;     // xy
//                               else if(2 == J + K) L = dim + 2; // xz
//                               else if(3 == J + K) L = dim + 1; // yz
//                               hessSol2DotN[I] += sol2[I][i] * normal[J] * nablaPhi2[i * dim2 + L] * normal[K];
//                             }
//                           }
//                         }
//                       }
// 
//                       double cNormL2 = 0.;
//                       if(fluid) {
//                         for(unsigned k = 0; k < dim; k++) {
//                           cNormL2 += aleVel[k] * aleVel[k];
//                         }
//                         cNormL2 = sqrt(cNormL2);
//                       }
// 
//                       double phiT1 = 1;//(mu / rho + (1. / 6.) * cNormL2 * h + (1. / 12.) * h * h / (par->_theta * dt)); //[velocity * h]
//                       double phiT2 = 1;//(mu / rho + (1. / 6.) * cNormL2 * h + (1. / 12.) * h * h / (par->_theta * dt)); //[velocity * h]
// 
//                       double phiC = 1;//0.5 * h * h * (1. / phiT1 + 1. / phiT2); // [h/velocity]
// 
//                       double C1 = 1;//(fluid) ? par->_gammacF * (mu + rho * phiC * cNormL2 * cNormL2 + rho * h2 / (par->_theta * dt)) :   par->_gammacS * (mu + rho * h2 / (par->_theta * dt * dt));
// 
//                       // [mu_f] = Pa.s = F / h2 * s = kg / (s h)
//                       // [mu_s] = Pa = F / h2 * s = kg / (s^2 h)
//                       // [C1] for the fluid is [rho * velocity * h] = kg / h /s = kg/(s h)
//                       // [C1] for the solid is kg/(s^2 h)
// 
//                       double C2 = 1;//(fluid) ? par->_gammau * rho * phiC : 0.;
// 
//                       for(unsigned I = 0; I < dim; I++) {
//                         for(unsigned i = 0; i < nDofs1; i++) {
//                           if(fluid) aRes1[I][i] +=  C2 * h * gradPhi1[i * dim + I] * (divSol1g - divSol2g) * weight;
//                           for(unsigned J = 0; J < dim; J++) {
//                             aRes1[I][i] +=  C1 * h * gradPhi1[i * dim + J] * normal[J] * (gradSol1DotN[I] - gradSol2DotN[I]) * weight;
//                             for(unsigned K = 0; K < dim; K++) {
// 
//                               unsigned L;
//                               if(J == K) L = J;
//                               else if(1 == J + K) L = dim;     // xy
//                               else if(2 == J + K) L = dim + 2; // xz
//                               else if(3 == J + K) L = dim + 1; // yz
// 
//                               aRes1[I][i] += C1 * h3 * normal[J] * nablaPhi1[i * dim2 + L] * normal[K] * (hessSol1DotN[I] - hessSol2DotN[I]) * weight;
//                             }
//                           }
//                         }
// 
//                         for(unsigned i = 0; i < nDofs2; i++) {
//                           if(fluid) aRes2[I][i] +=  -C2 * h * gradPhi2[i * dim + I] * (divSol1g - divSol2g) * weight;
//                           for(unsigned J = 0; J < dim; J++) {
//                             aRes2[I][i] +=  -C1 * h * gradPhi2[i * dim + J] * normal[J] * (gradSol1DotN[I] - gradSol2DotN[I]) * weight;
// 
//                             for(unsigned K = 0; K < dim; K++) {
// 
//                               unsigned L;
//                               if(J == K) L = J;
//                               else if(1 == J + K) L = dim;     // xy
//                               else if(2 == J + K) L = dim + 2; // xz
//                               else if(3 == J + K) L = dim + 1; // yz
// 
//                               aRes2[I][i] +=  -C1 * h3 * normal[J] * nablaPhi2[i * dim2 + L] * normal[K] * (hessSol1DotN[I] - hessSol2DotN[I]) * weight;
//                             }
//                           }
//                         }
//                       }
// 
//                       if(fluid && solTypeP < 3) {
//                         msh->_finiteElement[ielt1][solTypeP]->Jacobian(vx1, xi1, weight1, phi1, gradPhi1);
//                         msh->_finiteElement[ielt2][solTypeP]->Jacobian(vx2, xi2, weight2, phi2, gradPhi2);
// 
//                         adept::adouble gradSolP1DotN = 0.;
//                         adept::adouble gradSolP2DotN = 0.;
// 
// 
//                         for(unsigned i = 0; i < nDofsP1; i++) {
//                           for(unsigned J = 0; J < dim; J++) {
//                             gradSolP1DotN += solP1[i] * gradPhi1[i * dim + J] * normal[J];
//                           }
//                         }
// 
//                         for(unsigned i = 0; i < nDofsP2; i++) {
//                           for(unsigned J = 0; J < dim; J++) {
//                             gradSolP2DotN += solP2[i] * gradPhi2[i * dim + J] * normal[J];
//                           }
//                         }
// 
//                         for(unsigned i = 0; i < nDofsP1; i++) {
//                           for(unsigned J = 0; J < dim; J++) {
//                             //aResP1[i] +=  par->_gammap * phiC / rho * h *  gradPhi1[i * dim + J] * normal[J] * (gradSolP1DotN - gradSolP2DotN) * weight;
//                           }
//                         }
// 
//                         for(unsigned i = 0; i < nDofsP2; i++) {
//                           for(unsigned J = 0; J < dim; J++) {
//                             //aResP2[i] +=  - par->_gammap * phiC / rho * h * gradPhi2[i * dim + J] * normal[J] * (gradSolP1DotN - gradSolP2DotN) * weight;
//                           }
//                         }
// 
//                       }
// 
//                     }
// 
//                     //copy the value of the adept::adoube aRes in double Res and store them in RES
//                     rhs1.resize(nDofsAll1);   //resize
//                     for(int i = 0; i < nDofs1; i++) {
//                       for(unsigned  k = 0; k < dim; k++) {
//                         rhs1[ k * nDofs1 + i ] = -aRes1[k][i].value();
//                       }
//                     }
//                     for(int i = 0; i < nDofsP1; i++) {
//                       rhs1[ dim * nDofs1 + i] = -aResP1[i].value();
//                     }
//                     myRES->add_vector_blocked(rhs1, sysDofs1);
// 
//                     rhs2.resize(nDofsAll2);   //resize
//                     for(int i = 0; i < nDofs2; i++) {
//                       for(unsigned  k = 0; k < dim; k++) {
//                         rhs2[ k * nDofs2 + i ] = -aRes2[k][i].value();
//                       }
//                     }
//                     for(int i = 0; i < nDofsP2; i++) {
//                       rhs2[ dim * nDofs2 + i] = -aResP2[i].value();
//                     }
//                     myRES->add_vector_blocked(rhs2, sysDofs2);
// 
//                     // define the dependent variables J11 and J12
//                     for(unsigned  k = 0; k < dim; k++) {
//                       s.dependent(&aRes1[k][0], nDofs1);
//                     }
//                     s.dependent(&aResP1[0], nDofsP1);
// 
//                     // define the independent variables J11
//                     for(unsigned  k = 0; k < dim; k++) {
//                       s.independent(&sol1[k][0], nDofs1);
//                     }
//                     s.independent(&solP1[0], nDofsP1);
//                     Jac.resize(nDofsAll1 * nDofsAll1);
//                     // get the and store jacobian matrix (row-major)
//                     s.jacobian(&Jac[0], true);
//                     myKK->add_matrix_blocked(Jac, sysDofs1, sysDofs1);
//                     s.clear_independents();
// 
//                     // define the independent variables J12
//                     for(unsigned  k = 0; k < dim; k++) {
//                       s.independent(&sol2[k][0], nDofs2);
//                     }
//                     s.independent(&solP2[0], nDofsP2);
//                     Jac.resize(nDofsAll1 * nDofsAll2);
//                     // get the and store jacobian matrix (row-major)
//                     s.jacobian(&Jac[0], true);
//                     myKK->add_matrix_blocked(Jac, sysDofs1, sysDofs2);
//                     s.clear_independents();
// 
//                     s.clear_dependents(); // for J11 and J12
// 
//                     // define the dependent variables J21 and J22
//                     for(unsigned  k = 0; k < dim; k++) {
//                       s.dependent(&aRes2[k][0], nDofs2);
//                     }
//                     s.dependent(&aResP2[0], nDofsP2);
// 
//                     // define the independent variables J21
//                     for(unsigned  k = 0; k < dim; k++) {
//                       s.independent(&sol1[k][0], nDofs1);
//                     }
//                     s.independent(&solP1[0], nDofsP1);
//                     Jac.resize(nDofsAll2 * nDofsAll1);
//                     // get the and store jacobian matrix (row-major)
//                     s.jacobian(&Jac[0], true);
//                     myKK->add_matrix_blocked(Jac, sysDofs2, sysDofs1);
//                     s.clear_independents();
// 
//                     // define the independent variables J22
//                     for(unsigned  k = 0; k < dim; k++) {
//                       s.independent(&sol2[k][0], nDofs2);
//                     }
//                     s.independent(&solP2[0], nDofsP2);
//                     Jac.resize(nDofsAll2 * nDofsAll2);
//                     // get the and store jacobian matrix (row-major)
//                     s.jacobian(&Jac[0], true);
//                     myKK->add_matrix_blocked(Jac, sysDofs2, sysDofs2);
//                     s.clear_independents();
// 
//                     s.clear_dependents(); // for J21 and J22
//                   }
//                 }
//               }
//             }
//           }
//         }
//       }
//     }
//   }
  // *************************************
  std::cout << "Ghost Penalty Assembly time = " << static_cast<double>(clock() - start_time) / CLOCKS_PER_SEC << std::endl;

}




