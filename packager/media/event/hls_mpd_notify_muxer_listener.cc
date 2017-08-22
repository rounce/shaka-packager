// Copyright 2014 Google Inc. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#include "packager/media/event/hls_mpd_notify_muxer_listener.h"

namespace shaka {
namespace media {

HlsMpdNotifyMuxerListener::HlsMpdNotifyMuxerListener(const std::string& playlist_name,
                         const std::string& ext_x_media_name,
                         const std::string& ext_x_media_group_id,
                         hls::HlsNotifier* hls_notifier,
                         MpdNotifier* mpd_notifier) {
    mpd_notify_muxer_listener.reset(new MpdNotifyMuxerListener(mpd_notifier));
    hls_notify_muxer_listener.reset(new HlsNotifyMuxerListener(playlist_name, ext_x_media_name,
                                                      ext_x_media_group_id, hls_notifier));
}

HlsMpdNotifyMuxerListener::~HlsMpdNotifyMuxerListener() {}

void HlsMpdNotifyMuxerListener::OnEncryptionInfoReady(
    bool is_initial_encryption_info,
    FourCC protection_scheme,
    const std::vector<uint8_t>& key_id,
    const std::vector<uint8_t>& iv,
    const std::vector<ProtectionSystemSpecificInfo>& key_system_info) {
    mpd_notify_muxer_listener->OnEncryptionInfoReady(is_initial_encryption_info, protection_scheme, key_id, iv, key_system_info);
    hls_notify_muxer_listener->OnEncryptionInfoReady(is_initial_encryption_info, protection_scheme, key_id, iv, key_system_info);
}

void HlsMpdNotifyMuxerListener::OnEncryptionStart() {
    mpd_notify_muxer_listener->OnEncryptionStart();
    hls_notify_muxer_listener->OnEncryptionStart();
}

void HlsMpdNotifyMuxerListener::OnMediaStart(
    const MuxerOptions& muxer_options,
    const StreamInfo& stream_info,
    uint32_t time_scale,
    ContainerType container_type) {
    mpd_notify_muxer_listener->OnMediaStart(muxer_options, stream_info, time_scale, container_type);
    hls_notify_muxer_listener->OnMediaStart(muxer_options, stream_info, time_scale, container_type);
}

// Record the sample duration in the media info for VOD so that OnMediaEnd, all
// the information is in the media info.
void HlsMpdNotifyMuxerListener::OnSampleDurationReady(
    uint32_t sample_duration) {
    mpd_notify_muxer_listener->OnSampleDurationReady(sample_duration);
    hls_notify_muxer_listener->OnSampleDurationReady(sample_duration);
}

void HlsMpdNotifyMuxerListener::OnMediaEnd(const MediaRanges& media_ranges,
                                        float duration_seconds) {
    mpd_notify_muxer_listener->OnMediaEnd(media_ranges, duration_seconds);
    hls_notify_muxer_listener->OnMediaEnd(media_ranges, duration_seconds);
}

void HlsMpdNotifyMuxerListener::OnNewSegment(const std::string& file_name,
                                          uint64_t start_time,
                                          uint64_t duration,
                                          uint64_t segment_file_size) {
    mpd_notify_muxer_listener->OnNewSegment(file_name, start_time, duration, segment_file_size);
    hls_notify_muxer_listener->OnNewSegment(file_name, start_time, duration, segment_file_size);
}

}  // namespace media
}  // namespace shaka
