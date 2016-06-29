/**
 *   Copyright 2011-2015 Quickstep Technologies LLC.
 *   Copyright 2015-2016 Pivotal Software, Inc.
 *   Copyright 2016, Quickstep Research Group, Computer Sciences Department,
 *     University of Wisconsin—Madison.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 **/

#ifndef QUICKSTEP_RELATIONAL_OPERATORS_WINDOW_AGGREGATION_OPERATOR_HPP_
#define QUICKSTEP_RELATIONAL_OPERATORS_WINDOW_AGGREGATION_OPERATOR_HPP_

#include <vector>

#include "catalog/CatalogRelation.hpp"
#include "query_execution/QueryContext.hpp"
#include "relational_operators/RelationalOperator.hpp"
#include "relational_operators/WorkOrder.hpp"
#include "storage/StorageBlockInfo.hpp"
#include "utility/Macros.hpp"

#include "glog/logging.h"

#include "tmb/id_typedefs.h"

namespace tmb { class MessageBus; }

namespace quickstep {

class StorageManager;
class WorkOrderProtosContainer;
class WorkOrdersContainer;

namespace serialization { class WorkOrder; }

/** \addtogroup RelationalOperators
 *  @{
 */

/**
 * @brief An operator which performs window aggregation over a relation.
 **/
class WindowAggregationOperator : public RelationalOperator {
 public:
  /**
   * @brief Constructor.
   *
   * @param query_id The ID of this query.
   * @param input_relation The relation to perform aggregation over.
   **/
  WindowAggregationOperator(const std::size_t query_id,
                            const CatalogRelation &input_relation)
      : RelationalOperator(query_id),
        input_relation_(input_relation),
        generated_(false) {}

  ~WindowAggregationOperator() override {}

  bool getAllWorkOrders(WorkOrdersContainer *container,
                        QueryContext *query_context,
                        StorageManager *storage_manager,
                        const tmb::client_id scheduler_client_id,
                        tmb::MessageBus *bus) override;

  bool getAllWorkOrderProtos(WorkOrderProtosContainer *container) override;

 private:
  /**
   * @brief Create Work Order proto.
   *
   * @return A window aggregation work order.
   **/
  serialization::WorkOrder* createWorkOrderProto();

  const CatalogRelation &input_relation_;
  bool generated_;

  DISALLOW_COPY_AND_ASSIGN(WindowAggregationOperator);
};

/**
 * @brief A WorkOrder produced by WindowAggregationOperator.
 **/
class WindowAggregationWorkOrder : public WorkOrder {
 public:
  /**
   * @brief Constructor
   *
   * @param query_id The ID of this query.
   * @param input_block_id The block id.
   **/
  WindowAggregationWorkOrder(const std::size_t query_id,
                             const CatalogRelation &input_relation)
      : WorkOrder(query_id),
        input_relation_(input_relation)  {}

  ~WindowAggregationWorkOrder() override {}

  void execute() override;

 private:
  const CatalogRelation &input_relation_;

  DISALLOW_COPY_AND_ASSIGN(WindowAggregationWorkOrder);
};

/** @} */

}  // namespace quickstep

#endif  // QUICKSTEP_RELATIONAL_OPERATORS_WINDOW_AGGREGATION_OPERATOR_HPP_
