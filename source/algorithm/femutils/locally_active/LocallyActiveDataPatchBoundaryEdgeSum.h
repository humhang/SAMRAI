//
// File:	$URL: file:///usr/casc/samrai/repository/SAMRAI/tags/v-2-3-0/source/algorithm/femutils/locally_active/LocallyActiveDataPatchBoundaryEdgeSum.h $
// Package:	SAMRAI algorithms
// Copyright:	(c) 1997-2008 Lawrence Livermore National Security, LLC
// Revision:	$LastChangedRevision: 2132 $
// Modified:	$LastChangedDate: 2008-04-14 14:51:47 -0700 (Mon, 14 Apr 2008) $
// Description:	Routines for summing locally-active edge data at patch boundaries
//
 
#ifndef included_algs_LocallyActiveDataPatchBoundaryEdgeSum
#define included_algs_LocallyActiveDataPatchBoundaryEdgeSum

#include "SAMRAI_config.h"

#include "PatchLevel.h"
#include "LocallyActiveDataPatchLevelManager.h"
#include "EdgeVariable.h"
#include "OuteredgeVariable.h"
#include "tbox/Pointer.h"
#ifndef included_String
#include <string>
#define included_String
#endif
#include "LocallyActiveDataRefineSchedule.h"
#include "LocallyActiveDataRefineTransactionFactory.h"

namespace SAMRAI {
    namespace algs {

 /*!
 *  @brief Class LocallyActiveDataPatchBoundaryEdgeSum provides operations 
 *  summing locally-active edge data values at edges that are shared by 
 *  multiple patches on a single level.  Note that this utility only works 
 *  on a SINGLE patch level, not  on a multiple levels in an AMR patch 
 *  hierarchy like the LocallyActiveDataPatchBoundaryNodeSum class. Unlike 
 *  node data, edge data at coarse-fine boundaries are not co-located, so 
 *  the sum operation across levels depends on application requirements.
 *
 *  Usage of a patch boundry edge sum involves the following sequence of steps:
 *
 *  -# Construct a patch boundry edge sum object.  For example,
 *     \verbatim
 *         LocallyActiveDataPatchBoundaryEdgeSum<DIM> my_edge_sum("My Edge Sum");
 *     \endverbatim
 *  -# Register edge data quantities to sum.  For example,
 *     \verbatim
 *         my_edge_sum.registerSum(edge_data_id1);
 *         my_edge_sum.registerSum(edge_data_id2);
 *         etc...
 *     \endverbatim
 *  -# Setup the sum operations for a single level involving patches defined 
 *     to be active by a given level manager.  For example,
 *     \verbatim
 *         my_edge_sum.setupSum(level, level_mgr);
 *     \endverbatim
 *  -# Execute the sum operation.  For example,
 *     \verbatim
 *         my_edge_sum.computeSum()
 *     \endverbatim
 *
 *  The result of these operations is that each edge patch data value associated
 *  with the registered ids at patch boundaries on the level is replaced by the 
 *  sum of all data values at the edge.
 */

template<int DIM> class LocallyActiveDataPatchBoundaryEdgeSum
{
public:

   /*!
    *  @brief Static function used to predetermine number of patch data
    *         slots ahared among all LocallyActiveDataPatchBoundaryEdgeSum
    *         objects (i.e., static members).  To get a correct count,
    *         this routine should only be called once.
    *
    *  @return integer number of internal patch data slots required
    *          to perform sum.
    *  @param max_variables_to_register integer value indicating
    *          maximum number of patch data ids that will be registered  
    *          with edge sum objects.
    */
   static int getNumSharedPatchDataSlots(int max_variables_to_register);

   /*!
    *  @brief Static function used to predetermine number of patch data
    *         slots unique to each LocallyActiveDataPatchBoundaryEdgeSum
    *         object (i.e., non-static members).  To get a correct count,
    *         this routine should be called exactly once for each object
    *         that will be constructed.
    *
    *  @return integer number of internal patch data slots required
    *          to perform sum.
    *  @param max_variables_to_register integer value indicating
    *          maximum number of patch data ids that will be registered
    *          with edge sum objects.
    */
   static int getNumUniquePatchDataSlots(int max_variables_to_register);

   /*!
    *  @brief Constructor initializes object to default (mostly undefined) 
    *  state.
    *
    *  @param object_name const std::string reference for name of object used 
    *  in error reporting.  When assertion checking is on, the string
    *  cannot be empty.
    */
   LocallyActiveDataPatchBoundaryEdgeSum(const std::string& object_name);

   /*!
    *  @brief Destructor for the schedule releases all internal storage.
    */
   ~LocallyActiveDataPatchBoundaryEdgeSum<DIM>();

   /*!
    *  @brief Register edge data with given patch data identifier for summing.
    *
    *  @param edge_data_id  integer patch data index for edge data to sum
    *
    *  The edge data id must be a valid patch data id (>=0) and must
    *  correspond to edge-centered double data.  If not, an error will result.
    */
   void registerSum(int edge_data_id);

   /*!
    *  @brief Set up summation operations for edge data across shared edges
    *         on a single level.
    *
    *  @param level         pointer to level on which to perform edge sum
    *  @param level_mgr     pointer to level mgr defining active patches
    *                       for edge data registered with the edge sum object
    *
    *  When assertion checking is active, neither pointer can be null.
    */
   void setupSum(
      tbox::Pointer<hier::PatchLevel<DIM> > level,
      tbox::Pointer< hier::LocallyActiveDataPatchLevelManager<DIM> > level_mgr);

   /*!
    *  @brief Compute sum of edge values at each shared edge and replace 
    *         each such edge value with the corresponding sum.  
    *
    *  At the end of this method, all values at shared edge locations on 
    *  patch boundaries will have the same value.  
    */
   void computeSum() const;

private:

   /*
    * Private member function to perform edge sum across single level --
    * called from computeSum()
    */
   void doLevelSum(
      tbox::Pointer<hier::PatchLevel<DIM> > level,
      tbox::Pointer< hier::LocallyActiveDataPatchLevelManager<DIM> > level_mgr) const;

   /*
    * Private member function to set up management of internal work data --
    * called from setupSum()
    */
   void setInternalWorkDataActive(
      tbox::Pointer<hier::PatchLevel<DIM> > level,
      tbox::Pointer< hier::LocallyActiveDataPatchLevelManager<DIM> > level_mgr);

   /*
    * Static members for managing shared temporary data among multiple
    * LocallyActiveDataPatchBoundaryEdgeSum objects.
    */
   static int s_instance_counter;
   // These arrays are indexed [data depth][number of variables with depth]
   static tbox::Array< tbox::Array<int> > s_oedge_src_id_array;
   static tbox::Array< tbox::Array<int> > s_oedge_dst_id_array;

   enum PATCH_BDRY_EDGE_SUM_DATA_ID { ID_UNDEFINED = -1 };

   std::string d_object_name;
   bool d_setup_called;

   int d_num_reg_sum;

   // These arrays are indexed [variable registration sequence number]
   tbox::Array<int> d_user_edge_data_id;
   tbox::Array<int> d_user_edge_depth;

   // These arrays are indexed [data depth]
   tbox::Array<int> d_num_registered_data_by_depth;

   /*
    * Edge-centered variables and patch data indices used as internal work quantities.
    */
   // These arrays are indexed [variable registration sequence number]
   tbox::Array< tbox::Pointer< hier::Variable<DIM> > > d_tmp_oedge_src_variable;
   tbox::Array< tbox::Pointer< hier::Variable<DIM> > > d_tmp_oedge_dst_variable;

   // These arrays are indexed [variable registration sequence number]
   tbox::Array<int> d_oedge_src_id;
   tbox::Array<int> d_oedge_dst_id;

   /*
    * Sets of indices for temporary variables to expedite allocation/deallocation.
    */
   hier::ComponentSelector d_oedge_src_data_set;
   hier::ComponentSelector d_oedge_dst_data_set;

   tbox::Pointer< hier::PatchLevel<DIM> > d_level;
   tbox::Pointer< hier::LocallyActiveDataPatchLevelManager<DIM> > d_level_mgr;

   tbox::Pointer< xfer::LocallyActiveDataRefineTransactionFactory<DIM> > 
      d_sum_transaction_factory;
   
   tbox::Pointer< xfer::LocallyActiveDataRefineSchedule<DIM> > 
      d_single_level_sum_schedule;

};

}
}
#endif

#ifdef INCLUDE_TEMPLATE_IMPLEMENTATION
#include "LocallyActiveDataPatchBoundaryEdgeSum.C"
#endif

