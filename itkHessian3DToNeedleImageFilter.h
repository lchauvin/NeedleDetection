/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkHessian3DToNeedleImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2009-04-25 12:27:26 $
  Version:   $Revision: 1.9 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkHessian3DToNeedleImageFilter_h
#define __itkHessian3DToNeedleImageFilter_h

#include "itkSymmetricSecondRankTensor.h"
#include "itkSymmetricEigenAnalysisImageFilterWithMatrix.h"

namespace itk
{
/** \class Hessian3DToNeedleImageFilter
 * \brief 
 * Line filter to provide a vesselness measure for tubular objects from the
 * hessian matrix. The filter takes as input an image of hessian pixels
 * (SymmetricSecondRankTensor pixels) and preserves pixels that have
 * eigen values \f$ \lambda_3 \f$ close to 0 and \f$\lambda_2\f$ and \f$\lambda_1\f$ as
 * large negative values. (for bright tubular structures).
 * 
 * \f[ \lambda_1 < \lambda_2 < \lambda_3 \f]
 *
 * \par Notes:
 * The filter takes into account that the eigen values play a crucial role in 
 * discrimintaitng shape and orientation of structures. 
 *
 * \li Bright tubular structures will have low \f$\lambda_1\f$ and large negative 
 * values of \f$\lambda_2\f$ and \f$\lambda_3\f$.
 * \li Conversely dark tubular structures will have a low value of 
 * \f$\lambda_1\f$ and large positive values of \f$\lambda_2\f$ and 
 * \f$\lambda_3\f$. 
 * \li Bright plate like structures have low values of \f$\lambda_1\f$ and 
 * \f$\lambda_2\f$ and large negative values of \f$\lambda_3\f$
 * \li Dark plate like structures have low values of \f$\lambda_1\f$ and 
 * \f$\lambda_2\f$ and large positive values of \f$\lambda_3\f$
 * \li Bright spherical (blob) like structures have all three eigen values as
 * large negative numbers
 * \li Dark spherical (blob) like structures have all three eigen values as
 * large positive numbers
 *  
 * This filter is used to discriminate the Bright tubular structures.
 *
 * \par References: 
 * "3D Multi-scale line filter for segmentation and visualization of 
 * curvilinear structures in medical images", 
 * Yoshinobu Sato, Shin Nakajima, Hideki Atsumi, Thomas Koller,
 * Guido Gerig, Shigeyuki Yoshida, Ron Kikinis.
 *
 * http://www.spl.harvard.edu/pages/spl-pre2007/pages/papers/yoshi
 * 
 *
 * \sa HessianRecursiveGaussianImageFilter 
 * \sa SymmetricEigenAnalysisImageFilter
 * \sa SymmetricSecondRankTensor
 * 
 * \ingroup IntensityImageFilters TensorObjects
 *
 */
  
template < typename  TPixel >
class ITK_EXPORT Hessian3DToNeedleImageFilter : public
ImageToImageFilter< Image< SymmetricSecondRankTensor< double, 3 >, 3 >, 
                                                  Image< TPixel, 3 > >
{
public:
  /** Standard class typedefs. */
  typedef Hessian3DToNeedleImageFilter Self;
  typedef ImageToImageFilter< 
          Image< SymmetricSecondRankTensor< double, 3 >, 3 >, 
          Image< TPixel, 3 > >                    Superclass;
  typedef SmartPointer<Self>                      Pointer;
  typedef SmartPointer<const Self>                ConstPointer;
  
  typedef typename Superclass::InputImageType            InputImageType;
  typedef typename Superclass::OutputImageType           OutputImageType;
  typedef typename InputImageType::PixelType             InputPixelType;
  typedef TPixel                                         OutputPixelType;
  
  /** Image dimension = 3. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      ::itk::GetImageDimension<InputImageType>::ImageDimension);
  itkStaticConstMacro(InputPixelDimension, unsigned int,
                      InputPixelType::Dimension);

  typedef  FixedArray< double, itkGetStaticConstMacro(InputPixelDimension) >
                                                          EigenValueArrayType;
  typedef  Image< EigenValueArrayType, itkGetStaticConstMacro(ImageDimension) >
                                                          EigenValueImageType;
  typedef   SymmetricEigenAnalysisImageFilterWithMatrix< 
              InputImageType, EigenValueImageType >     EigenAnalysisFilterType;

  /** Run-time type information (and related methods).   */
  itkTypeMacro( Hessian3DToNeedleImageFilter, ImageToImageFilter );

  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** Set/Get macros for alpha_1. Please refer to
   * http://www.spl.harvard.edu/pages/spl-pre2007/pages/papers/yoshi */
  itkSetMacro(Alpha1, double);
  itkGetConstMacro(Alpha1, double);
  
  /** Set/Get macros for alpha_2. Please refer to
   * http://www.spl.harvard.edu/pages/spl-pre2007/pages/papers/yoshi */
  itkSetMacro(Alpha2, double);
  itkGetConstMacro(Alpha2, double);

  itkSetMacro(AngleThreshold, double);
  itkGetConstMacro(AngleThreshold, double);

  itkSetMacro(LineSimilarityThreshold, double);
  itkGetConstMacro(LineSimilarityThreshold, double);

  inline void SetNormal(double x, double y, double z)
  {
    m_Normal[0] = x; m_Normal[1] = y; m_Normal[2] = z;
  }

  inline void GetNormal(double& x, double& y, double& z)
  {
    x = m_Normal[0]; y = m_Normal[1]; z = m_Normal[2];
  }

#ifdef ITK_USE_CONCEPT_CHECKING
  /** Begin concept checking */
  itkConceptMacro(DoubleConvertibleToOutputCheck,
                  (Concept::Convertible<double, OutputPixelType>));
  /** End concept checking */
#endif

protected:
  Hessian3DToNeedleImageFilter();
  ~Hessian3DToNeedleImageFilter() {};
  void PrintSelf(std::ostream& os, Indent indent) const;
  
  /** Generate Data */
  void GenerateData( void );

private:
  Hessian3DToNeedleImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  typename EigenAnalysisFilterType::Pointer m_SymmetricEigenValueFilter;

  double m_Alpha1;
  double m_Alpha2;
  double m_AngleThreshold;
  double m_LineSimilarityThreshold;
  double m_Normal[3];
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkHessian3DToNeedleImageFilter.txx"
#endif
  
#endif
