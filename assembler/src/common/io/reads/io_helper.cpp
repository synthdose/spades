//***************************************************************************
//* Copyright (c) 2019 Saint Petersburg State University
//* All Rights Reserved
//* See file LICENSE for details.
//***************************************************************************

#include "io_helper.hpp"

#include "file_reader.hpp"
#include "paired_readers.hpp"
#include "multifile_reader.hpp"
#include "converting_reader_wrapper.hpp"
#include "longest_valid_wrapper.hpp"
#include "rc_reader_wrapper.hpp"
#include "async_read_stream.hpp"

namespace io {

SingleStream EasyStream(const std::string& filename, bool followed_by_rc,
                        bool handle_Ns, OffsetType offset_type,
                        ThreadPool::ThreadPool *pool) {
    SingleStream reader  = (pool ?
                            make_async_stream<FileReadStream>(*pool, filename, offset_type) :
                            FileReadStream(filename, offset_type));
    if (handle_Ns)
        reader = LongestValidWrap<SingleRead>(std::move(reader));
    if (followed_by_rc)
        reader = RCWrap<SingleRead>(std::move(reader));

    return reader;
}

PairedStream EasyWrapPairedStream(PairedStream stream,
                                  bool followed_by_rc,
                                  LibraryOrientation orientation) {
    PairedStream reader{OrientationChangingWrapper<PairedRead>(std::move(stream), orientation)};
    reader = LongestValidWrap<PairedRead>(std::move(reader));
    if (followed_by_rc)
        reader = RCWrap<PairedRead>(std::move(reader));

    return reader;
}

PairedStream PairedEasyStream(const std::string& filename1, const std::string& filename2,
                              bool followed_by_rc, size_t insert_size,
                              bool use_orientation, LibraryOrientation orientation,
                              OffsetType offset_type,
                              ThreadPool::ThreadPool *pool) {
    return EasyWrapPairedStream(SeparatePairedReadStream(filename1, filename2, insert_size, offset_type,
                                                         pool),
                                followed_by_rc,
                                use_orientation ? orientation : LibraryOrientation::Undefined);
}

PairedStream PairedEasyStream(const std::string& filename, bool followed_by_rc,
                              size_t insert_size,
                              bool use_orientation, LibraryOrientation orientation,
                              OffsetType offset_type,
                              ThreadPool::ThreadPool *pool) {
    return EasyWrapPairedStream(InterleavingPairedReadStream(filename, insert_size, offset_type, pool), followed_by_rc,
                                use_orientation ? orientation : LibraryOrientation::Undefined);
}

}

