// Copyright 2017 Google Inc. All rights reserved.
//
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file or at
// https://developers.google.com/open-source/licenses/bsd

#ifndef PACKAGER_PACKAGER_H_
#define PACKAGER_PACKAGER_H_

#include <memory>
#include <string>
#include <vector>

#include "packager/hls/public/hls_params.h"
#include "packager/media/public/chunking_params.h"
#include "packager/media/public/crypto_params.h"
#include "packager/media/public/mp4_output_params.h"
#include "packager/mpd/public/mpd_params.h"
#include "packager/status.h"

namespace shaka {

/// Parameters used for testing.
struct TestParams {
  /// Whether to dump input stream info.
  bool dump_stream_info = false;
  /// Inject a fake clock which always returns 0. This allows deterministic
  /// output from packaging.
  bool inject_fake_clock = false;
  /// Inject and replace the library version string if specified, which is used
  /// to populate the version string in the manifests / media files.
  std::string injected_library_version;
};

/// Packaging parameters.
struct PackagingParams {
  /// Specify temporary directory for intermediate temporary files.
  std::string temp_dir;
  /// MP4 (ISO-BMFF) output related parameters.
  Mp4OutputParams mp4_output_params;
  /// Chunking (segmentation) related parameters.
  ChunkingParams chunking_params;

  /// Manifest generation related parameters. Right now only one of
  /// `output_media_info`, `mpd_params` and `hls_params` should be set. Create a
  /// human readable format of MediaInfo. The output file name will be the name
  /// specified by output flag, suffixed with `.media_info`.
  bool output_media_info = false;
  /// DASH MPD related parameters.
  MpdParams mpd_params;
  /// HLS related parameters.
  HlsParams hls_params;

  /// Encryption and Decryption Parameters.
  EncryptionParams encryption_params;
  DecryptionParams decryption_params;

  // Parameters for testing. Do not use in production.
  TestParams test_params;
};

/// Defines a single input/output stream.
struct StreamDescriptor {
  /// Input/source media file path or network stream URL. Required.
  std::string input;
  // TODO(kqyang): Add support for feeding data through read func.
  // std::function<int64_t(void* buffer, uint64_t length)> read_func;

  /// Stream selector, can be `audio`, `video`, `text` or a zero based stream
  /// index. Required.
  std::string stream_selector;

  /// Specifies output file path or init segment path (if segment template is
  /// specified). Can be empty for self initialization media segments.
  std::string output;
  /// Specifies segment template. Can be empty.
  std::string segment_template;
  // TODO: Add support for writing data through write func.
  // std::function<int64_t(const std::string& id, void* buffer, uint64_t
  // length)> write_func;

  /// Optional value which specifies output container format, e.g. "mp4". If not
  /// specified, will detect from output / segment template name.
  std::string output_format;
  /// If set to true, the stream will not be encrypted. This is useful, e.g. to
  /// encrypt only video streams.
  bool skip_encryption = false;
  /// If set to a non-zero value, will generate a trick play / trick mode
  /// stream with frames sampled from the key frames in the original stream.
  /// `trick_play_factor` defines the sampling rate.
  uint32_t trick_play_factor = 0;
  /// Optional user-specified content bit rate for the stream, in bits/sec.
  /// If specified, this value is propagated to the `$Bandwidth$` template
  /// parameter for segment names. If not specified, its value may be estimated.
  uint32_t bandwidth = 0;
  /// Optional value which contains a user-specified language tag. If specified,
  /// this value overrides any language metadata in the input stream.
  std::string language;
  /// Required for audio when outputting HLS. It defines the name of the output
  /// stream, which is not necessarily the same as output. This is used as the
  /// `NAME` attribute for EXT-X-MEDIA.
  std::string hls_name;
  /// Required for audio when outputting HLS. It defines the group ID for the
  /// output stream. This is used as the GROUP-ID attribute for EXT-X-MEDIA.
  std::string hls_group_id;
  /// Required for HLS output. It defines the name of the playlist for the
  /// stream. Usually ends with `.m3u8`.
  std::string hls_playlist_name;
};

class SHAKA_EXPORT Packager {
 public:
  Packager();
  ~Packager();

  /// Initialize packaging pipeline.
  /// @param packaging_params contains the packaging parameters.
  /// @param stream_descriptors a list of stream descriptors.
  /// @return OK on success, an appropriate error code on failure.
  Status Initialize(
      const PackagingParams& packaging_params,
      const std::vector<StreamDescriptor>& stream_descriptors);

  /// Run the pipeline to completion (or failed / been cancelled). Note
  /// that it blocks until completion.
  /// @return OK on success, an appropriate error code on failure.
  Status Run();

  /// Cancel packaging. Note that it has to be called from another thread.
  void Cancel();

  /// @return The version of the library.
  static std::string GetLibraryVersion();

  /// Default stream label function implementation.
  /// @param max_sd_pixels The threshold to determine whether a video track
  ///                      should be considered as SD. If the max pixels per
  ///                      frame is no higher than max_sd_pixels, i.e. [0,
  ///                      max_sd_pixels], it is SD.
  /// @param max_hd_pixels The threshold to determine whether a video track
  ///                      should be considered as HD. If the max pixels per
  ///                      frame is higher than max_sd_pixels, but no higher
  ///                      than max_hd_pixels, i.e. (max_sd_pixels,
  ///                      max_hd_pixels], it is HD.
  /// @param max_uhd1_pixels The threshold to determine whether a video track
  ///                        should be considered as UHD1. If the max pixels
  ///                        per frame is higher than max_hd_pixels, but no
  ///                        higher than max_uhd1_pixels, i.e. (max_hd_pixels,
  ///                        max_uhd1_pixels], it is UHD1. Otherwise it is
  ///                        UHD2.
  /// @param stream_info Encrypted stream info.
  /// @return the stream label associated with `stream_info`. Can be "AUDIO",
  ///         "SD", "HD", "UHD1" or "UHD2".
  static std::string DefaultStreamLabelFunction(
      int max_sd_pixels,
      int max_hd_pixels,
      int max_uhd1_pixels,
      const EncryptionParams::EncryptedStreamAttributes& stream_attributes);

 private:
  Packager(const Packager&) = delete;
  Packager& operator=(const Packager&) = delete;

  struct PackagerInternal;
  std::unique_ptr<PackagerInternal> internal_;
};

}  // namespace shaka

#endif  // PACKAGER_PACKAGER_H_
