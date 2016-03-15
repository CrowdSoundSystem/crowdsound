#ifndef crowdsoundserver_hpp
#define crowdsoundserver_hpp

#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <grpc++/grpc++.h>
#include "proto/crowdsound_service.grpc.pb.h"
#include "proto/crowdsound_admin_service.grpc.pb.h"

#include "playsource_client.hpp"
#include "skrillex/skrillex.hpp"
#include "DecisionAlgorithm.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerWriter;
using grpc::ServerReader;
using grpc::Status;

using CrowdSound::PingRequest;
using CrowdSound::PingResponse;
using CrowdSound::GetSessionDataRequest;
using CrowdSound::GetSessionDataResponse;
using CrowdSound::GetPlayingRequest;
using CrowdSound::GetPlayingResponse;
using CrowdSound::GetQueueRequest;
using CrowdSound::GetQueueResponse;
using CrowdSound::ListTrendingArtistsRequest;
using CrowdSound::ListTrendingArtistsResponse;
using CrowdSound::PostSongRequest;
using CrowdSound::PostSongResponse;
using CrowdSound::VoteSongRequest;
using CrowdSound::VoteSongResponse;
using CrowdSound::VoteSkipRequest;
using CrowdSound::VoteSkipResponse;

using CrowdSound::SkipStatusRequest;
using CrowdSound::SkipStatusResponse;
using CrowdSound::SkipRequest;
using CrowdSound::SkipResponse;

class CrowdSoundAdminImpl;

class CrowdSoundImpl final : public CrowdSound::CrowdSound::Service {
public:
    explicit CrowdSoundImpl(
        std::shared_ptr<skrillex::DB> db,
        std::unique_ptr<PlaysourceClient> playsource,
        std::shared_ptr<DecisionAlgorithm> decisionAlgorithm);

    Status Ping(ServerContext* context, const PingRequest* request, PingResponse* resp) override;
    Status GetSessionData(ServerContext* context, const GetSessionDataRequest* request, GetSessionDataResponse* resp) override;
    Status GetPlaying(ServerContext* context, const GetPlayingRequest* request, GetPlayingResponse* writer) override;
    Status GetQueue(ServerContext* context, const GetQueueRequest* request, ServerWriter<GetQueueResponse>* writer) override;
    Status ListTrendingArtists(ServerContext* context, const ListTrendingArtistsRequest* request, ServerWriter<ListTrendingArtistsResponse>* writer) override;
    Status PostSong(ServerContext* context, ServerReader<PostSongRequest>* request, PostSongResponse* resp) override;
    Status VoteSong(ServerContext* context, const VoteSongRequest* request, VoteSongResponse* resp) override;
    Status VoteSkip(ServerContext* context, const VoteSkipRequest* request, VoteSkipResponse* resp) override;

protected:
    void runPlaySource();
private:
    std::shared_ptr<skrillex::DB> db_;
    std::shared_ptr<skrillex::Mapper> mapper_;
    std::shared_ptr<DecisionAlgorithm> algo_;
    std::unique_ptr<PlaysourceClient> playsource_;

    std::mutex            skip_guard_;
    std::set<std::string> skip_voters_;

    std::thread ps_thread_;

    friend class CrowdSoundAdminImpl;
};

#endif