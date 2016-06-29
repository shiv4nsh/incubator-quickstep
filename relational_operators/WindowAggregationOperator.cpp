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
  if (!generated_) {
    container->addNormalWorkOrder(
        new WindowAggregationWorkOrder(query_id_, input_relation_), op_index_);
    generated_ = true;
  }

  return true;
}

bool WindowAggregationOperator::getAllWorkOrderProtos(WorkOrderProtosContainer *container) {
  if (!generated_) {
    container->addWorkOrderProto(createWorkOrderProto(), op_index_);
    generated_ = true;
  }

  return true;
}

serialization::WorkOrder* WindowAggregationOperator::createWorkOrderProto() {
  serialization::WorkOrder *proto = new serialization::WorkOrder;
  proto->set_work_order_type(serialization::WINDOW_AGGREGATION);
  proto->set_query_id(query_id_);

  std::vector<block_id> relation_blocks(input_relation_.getBlocksSnapshot());
  for (const block_id relation_block : relation_blocks) {
    proto->AddExtension(serialization::WindowAggregationWorkOrder::block_ids,
                        relation_block);
  }

  return proto;
}


void WindowAggregationWorkOrder::execute() {
  THROW_SQL_ERROR()
      << "Window aggregate function is not supported yet :(";
}

}  // namespace quickstep
