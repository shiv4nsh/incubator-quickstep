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

#include "relational_operators/WindowAggregationOperator.hpp"

#include <vector>

#include "query_execution/QueryContext.hpp"
#include "query_execution/WorkOrderProtosContainer.hpp"
#include "query_execution/WorkOrdersContainer.hpp"
#include "relational_operators/WorkOrder.pb.h"
#include "storage/StorageBlockInfo.hpp"
#include "utility/SqlError.hpp"

#include "tmb/id_typedefs.h"

namespace quickstep {

bool WindowAggregationOperator::getAllWorkOrders(
    WorkOrdersContainer *container,
    QueryContext *query_context,
    StorageManager *storage_manager,
    const tmb::client_id scheduler_client_id,
    tmb::MessageBus *bus) {
  DCHECK(query_context != nullptr);
  
  if (blocking_dependencies_met_ && !generated_) {
    container->addNormalWorkOrder(
        new WindowAggregationWorkOrder(
            query_id_,
            query_context->releaseWindowAggregationState(window_aggregation_state_index_),
            query_context->getInsertDestination(output_destination_index_)),
        op_index_);
    generated_ = true;
  }

  return generated_;
}

bool WindowAggregationOperator::getAllWorkOrderProtos(WorkOrderProtosContainer *container) {
  if (blocking_dependencies_met_ && !generated_) {
    container->addWorkOrderProto(createWorkOrderProto(), op_index_);
    generated_ = true;
  }

  return generated_;
}

serialization::WorkOrder* WindowAggregationOperator::createWorkOrderProto() {
  serialization::WorkOrder *proto = new serialization::WorkOrder;
  proto->set_work_order_type(serialization::WINDOW_AGGREGATION);
  proto->set_query_id(query_id_);
  proto->SetExtension(serialization::WindowAggregationWorkOrder::window_aggr_state_index,
                      window_aggregation_state_index_);
  proto->SetExtension(serialization::WindowAggregationWorkOrder::insert_destination_index,
                      output_destination_index_);

  return proto;
}


void WindowAggregationWorkOrder::execute() {
  THROW_SQL_ERROR()
      << "Window aggregate function is not supported yet :(";
}

}  // namespace quickstep
