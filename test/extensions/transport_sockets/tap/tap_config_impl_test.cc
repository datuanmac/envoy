#include "extensions/transport_sockets/tap/tap_config_impl.h"

#include "test/extensions/common/tap/common.h"

using testing::_;
using testing::Return;
using testing::ReturnRef;

namespace Envoy {
namespace Extensions {
namespace TransportSockets {
namespace Tap {
namespace {

namespace TapCommon = Extensions::Common::Tap;

class MockSocketTapConfig : public SocketTapConfig {
public:
  PerSocketTapperPtr createPerSocketTapper(uint64_t stream_id) override {
    return PerSocketTapperPtr{createPerSocketTapper_(stream_id)};
  }

  Extensions::Common::Tap::PerTapSinkHandleManagerPtr
  createPerTapSinkHandleManager(uint64_t trace_id) override {
    return Extensions::Common::Tap::PerTapSinkHandleManagerPtr{
        createPerTapSinkHandleManager_(trace_id)};
  }

  MOCK_METHOD1(createPerSocketTapper_, PerSocketTapper*(uint64_t stream_id));
  MOCK_METHOD1(createPerTapSinkHandleManager_,
               Extensions::Common::Tap::PerTapSinkHandleManager*(uint64_t trace_id));
  MOCK_CONST_METHOD0(maxBufferedRxBytes, uint32_t());
  MOCK_CONST_METHOD0(maxBufferedTxBytes, uint32_t());
  MOCK_CONST_METHOD0(createMatchStatusVector,
                     Extensions::Common::Tap::Matcher::MatchStatusVector());
  MOCK_CONST_METHOD0(rootMatcher, const Extensions::Common::Tap::Matcher&());
  MOCK_CONST_METHOD0(streaming, bool());
};

class PerSocketTapperImplTest : public testing::Test {
public:
  PerSocketTapperImplTest() {
    EXPECT_CALL(*config_, createPerTapSinkHandleManager_(1)).WillOnce(Return(sink_manager_));
    EXPECT_CALL(*config_, createMatchStatusVector())
        .WillOnce(Return(TapCommon::Matcher::MatchStatusVector(1)));
    EXPECT_CALL(*config_, rootMatcher()).WillRepeatedly(ReturnRef(matcher_));
    EXPECT_CALL(matcher_, onNewStream(_)).WillOnce(SaveArgAddress(&statuses_));
    tapper_ = std::make_unique<HttpPerRequestTapperImpl>(config_, 1);
  }

  std::shared_ptr<MockSocketTapConfig> config_{std::make_shared<MockSocketTapConfig>()};
  TapCommon::MockPerTapSinkHandleManager* sink_manager_ =
      new TapCommon::MockPerTapSinkHandleManager;
  std::unique_ptr<PerSocketTapperImpl> tapper_;
  std::vector<TapCommon::MatcherPtr> matchers_{1};
  TapCommon::MockMatcher matcher_{matchers_};
  TapCommon::Matcher::MatchStatusVector* statuses_;
};

}
}
}
}
}
