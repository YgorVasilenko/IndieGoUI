// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: editor_data.proto

#include "editor_data.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace editor_serialization {
PROTOBUF_CONSTEXPR region::region(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.x_)*/0
  , /*decltype(_impl_.y_)*/0
  , /*decltype(_impl_.w_)*/0
  , /*decltype(_impl_.h_)*/0
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct regionDefaultTypeInternal {
  PROTOBUF_CONSTEXPR regionDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~regionDefaultTypeInternal() {}
  union {
    region _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 regionDefaultTypeInternal _region_default_instance_;
PROTOBUF_CONSTEXPR CropsData::CropsData(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.crop_name_)*/{&::_pbi::fixed_address_empty_string, ::_pbi::ConstantInitialized{}}
  , /*decltype(_impl_.crop_)*/nullptr
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct CropsDataDefaultTypeInternal {
  PROTOBUF_CONSTEXPR CropsDataDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~CropsDataDefaultTypeInternal() {}
  union {
    CropsData _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 CropsDataDefaultTypeInternal _CropsData_default_instance_;
PROTOBUF_CONSTEXPR SerializedCropsData::SerializedCropsData(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.data_)*/{}
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct SerializedCropsDataDefaultTypeInternal {
  PROTOBUF_CONSTEXPR SerializedCropsDataDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~SerializedCropsDataDefaultTypeInternal() {}
  union {
    SerializedCropsData _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 SerializedCropsDataDefaultTypeInternal _SerializedCropsData_default_instance_;
}  // namespace editor_serialization
static ::_pb::Metadata file_level_metadata_editor_5fdata_2eproto[3];
static constexpr ::_pb::EnumDescriptor const** file_level_enum_descriptors_editor_5fdata_2eproto = nullptr;
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_editor_5fdata_2eproto = nullptr;

const uint32_t TableStruct_editor_5fdata_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::editor_serialization::region, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  ~0u,  // no _split_
  ~0u,  // no sizeof(Split)
  PROTOBUF_FIELD_OFFSET(::editor_serialization::region, _impl_.x_),
  PROTOBUF_FIELD_OFFSET(::editor_serialization::region, _impl_.y_),
  PROTOBUF_FIELD_OFFSET(::editor_serialization::region, _impl_.w_),
  PROTOBUF_FIELD_OFFSET(::editor_serialization::region, _impl_.h_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::editor_serialization::CropsData, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  ~0u,  // no _split_
  ~0u,  // no sizeof(Split)
  PROTOBUF_FIELD_OFFSET(::editor_serialization::CropsData, _impl_.crop_name_),
  PROTOBUF_FIELD_OFFSET(::editor_serialization::CropsData, _impl_.crop_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::editor_serialization::SerializedCropsData, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  ~0u,  // no _split_
  ~0u,  // no sizeof(Split)
  PROTOBUF_FIELD_OFFSET(::editor_serialization::SerializedCropsData, _impl_.data_),
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::editor_serialization::region)},
  { 12, -1, -1, sizeof(::editor_serialization::CropsData)},
  { 22, -1, -1, sizeof(::editor_serialization::SerializedCropsData)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::editor_serialization::_region_default_instance_._instance,
  &::editor_serialization::_CropsData_default_instance_._instance,
  &::editor_serialization::_SerializedCropsData_default_instance_._instance,
};

const char descriptor_table_protodef_editor_5fdata_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\021editor_data.proto\022\024editor_serializatio"
  "n\"4\n\006region\022\t\n\001x\030\001 \001(\002\022\t\n\001y\030\002 \001(\002\022\t\n\001w\030\003"
  " \001(\002\022\t\n\001h\030\004 \001(\002\"J\n\tCropsData\022\021\n\tcrop_nam"
  "e\030\001 \001(\t\022*\n\004crop\030\002 \001(\0132\034.editor_serializa"
  "tion.region\"D\n\023SerializedCropsData\022-\n\004da"
  "ta\030\001 \003(\0132\037.editor_serialization.CropsDat"
  "ab\006proto3"
  ;
static ::_pbi::once_flag descriptor_table_editor_5fdata_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_editor_5fdata_2eproto = {
    false, false, 249, descriptor_table_protodef_editor_5fdata_2eproto,
    "editor_data.proto",
    &descriptor_table_editor_5fdata_2eproto_once, nullptr, 0, 3,
    schemas, file_default_instances, TableStruct_editor_5fdata_2eproto::offsets,
    file_level_metadata_editor_5fdata_2eproto, file_level_enum_descriptors_editor_5fdata_2eproto,
    file_level_service_descriptors_editor_5fdata_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_editor_5fdata_2eproto_getter() {
  return &descriptor_table_editor_5fdata_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_editor_5fdata_2eproto(&descriptor_table_editor_5fdata_2eproto);
namespace editor_serialization {

// ===================================================================

class region::_Internal {
 public:
};

region::region(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:editor_serialization.region)
}
region::region(const region& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  region* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.x_){}
    , decltype(_impl_.y_){}
    , decltype(_impl_.w_){}
    , decltype(_impl_.h_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&_impl_.x_, &from._impl_.x_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.h_) -
    reinterpret_cast<char*>(&_impl_.x_)) + sizeof(_impl_.h_));
  // @@protoc_insertion_point(copy_constructor:editor_serialization.region)
}

inline void region::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.x_){0}
    , decltype(_impl_.y_){0}
    , decltype(_impl_.w_){0}
    , decltype(_impl_.h_){0}
    , /*decltype(_impl_._cached_size_)*/{}
  };
}

region::~region() {
  // @@protoc_insertion_point(destructor:editor_serialization.region)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void region::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void region::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void region::Clear() {
// @@protoc_insertion_point(message_clear_start:editor_serialization.region)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  ::memset(&_impl_.x_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.h_) -
      reinterpret_cast<char*>(&_impl_.x_)) + sizeof(_impl_.h_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* region::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // float x = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 13)) {
          _impl_.x_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr);
          ptr += sizeof(float);
        } else
          goto handle_unusual;
        continue;
      // float y = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 21)) {
          _impl_.y_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr);
          ptr += sizeof(float);
        } else
          goto handle_unusual;
        continue;
      // float w = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 29)) {
          _impl_.w_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr);
          ptr += sizeof(float);
        } else
          goto handle_unusual;
        continue;
      // float h = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 37)) {
          _impl_.h_ = ::PROTOBUF_NAMESPACE_ID::internal::UnalignedLoad<float>(ptr);
          ptr += sizeof(float);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* region::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:editor_serialization.region)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // float x = 1;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_x = this->_internal_x();
  uint32_t raw_x;
  memcpy(&raw_x, &tmp_x, sizeof(tmp_x));
  if (raw_x != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteFloatToArray(1, this->_internal_x(), target);
  }

  // float y = 2;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_y = this->_internal_y();
  uint32_t raw_y;
  memcpy(&raw_y, &tmp_y, sizeof(tmp_y));
  if (raw_y != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteFloatToArray(2, this->_internal_y(), target);
  }

  // float w = 3;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_w = this->_internal_w();
  uint32_t raw_w;
  memcpy(&raw_w, &tmp_w, sizeof(tmp_w));
  if (raw_w != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteFloatToArray(3, this->_internal_w(), target);
  }

  // float h = 4;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_h = this->_internal_h();
  uint32_t raw_h;
  memcpy(&raw_h, &tmp_h, sizeof(tmp_h));
  if (raw_h != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteFloatToArray(4, this->_internal_h(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:editor_serialization.region)
  return target;
}

size_t region::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:editor_serialization.region)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // float x = 1;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_x = this->_internal_x();
  uint32_t raw_x;
  memcpy(&raw_x, &tmp_x, sizeof(tmp_x));
  if (raw_x != 0) {
    total_size += 1 + 4;
  }

  // float y = 2;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_y = this->_internal_y();
  uint32_t raw_y;
  memcpy(&raw_y, &tmp_y, sizeof(tmp_y));
  if (raw_y != 0) {
    total_size += 1 + 4;
  }

  // float w = 3;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_w = this->_internal_w();
  uint32_t raw_w;
  memcpy(&raw_w, &tmp_w, sizeof(tmp_w));
  if (raw_w != 0) {
    total_size += 1 + 4;
  }

  // float h = 4;
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_h = this->_internal_h();
  uint32_t raw_h;
  memcpy(&raw_h, &tmp_h, sizeof(tmp_h));
  if (raw_h != 0) {
    total_size += 1 + 4;
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData region::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    region::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*region::GetClassData() const { return &_class_data_; }


void region::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<region*>(&to_msg);
  auto& from = static_cast<const region&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:editor_serialization.region)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_x = from._internal_x();
  uint32_t raw_x;
  memcpy(&raw_x, &tmp_x, sizeof(tmp_x));
  if (raw_x != 0) {
    _this->_internal_set_x(from._internal_x());
  }
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_y = from._internal_y();
  uint32_t raw_y;
  memcpy(&raw_y, &tmp_y, sizeof(tmp_y));
  if (raw_y != 0) {
    _this->_internal_set_y(from._internal_y());
  }
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_w = from._internal_w();
  uint32_t raw_w;
  memcpy(&raw_w, &tmp_w, sizeof(tmp_w));
  if (raw_w != 0) {
    _this->_internal_set_w(from._internal_w());
  }
  static_assert(sizeof(uint32_t) == sizeof(float), "Code assumes uint32_t and float are the same size.");
  float tmp_h = from._internal_h();
  uint32_t raw_h;
  memcpy(&raw_h, &tmp_h, sizeof(tmp_h));
  if (raw_h != 0) {
    _this->_internal_set_h(from._internal_h());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void region::CopyFrom(const region& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:editor_serialization.region)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool region::IsInitialized() const {
  return true;
}

void region::InternalSwap(region* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(region, _impl_.h_)
      + sizeof(region::_impl_.h_)
      - PROTOBUF_FIELD_OFFSET(region, _impl_.x_)>(
          reinterpret_cast<char*>(&_impl_.x_),
          reinterpret_cast<char*>(&other->_impl_.x_));
}

::PROTOBUF_NAMESPACE_ID::Metadata region::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_editor_5fdata_2eproto_getter, &descriptor_table_editor_5fdata_2eproto_once,
      file_level_metadata_editor_5fdata_2eproto[0]);
}

// ===================================================================

class CropsData::_Internal {
 public:
  static const ::editor_serialization::region& crop(const CropsData* msg);
};

const ::editor_serialization::region&
CropsData::_Internal::crop(const CropsData* msg) {
  return *msg->_impl_.crop_;
}
CropsData::CropsData(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:editor_serialization.CropsData)
}
CropsData::CropsData(const CropsData& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  CropsData* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.crop_name_){}
    , decltype(_impl_.crop_){nullptr}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _impl_.crop_name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.crop_name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (!from._internal_crop_name().empty()) {
    _this->_impl_.crop_name_.Set(from._internal_crop_name(), 
      _this->GetArenaForAllocation());
  }
  if (from._internal_has_crop()) {
    _this->_impl_.crop_ = new ::editor_serialization::region(*from._impl_.crop_);
  }
  // @@protoc_insertion_point(copy_constructor:editor_serialization.CropsData)
}

inline void CropsData::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.crop_name_){}
    , decltype(_impl_.crop_){nullptr}
    , /*decltype(_impl_._cached_size_)*/{}
  };
  _impl_.crop_name_.InitDefault();
  #ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
    _impl_.crop_name_.Set("", GetArenaForAllocation());
  #endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
}

CropsData::~CropsData() {
  // @@protoc_insertion_point(destructor:editor_serialization.CropsData)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void CropsData::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.crop_name_.Destroy();
  if (this != internal_default_instance()) delete _impl_.crop_;
}

void CropsData::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void CropsData::Clear() {
// @@protoc_insertion_point(message_clear_start:editor_serialization.CropsData)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.crop_name_.ClearToEmpty();
  if (GetArenaForAllocation() == nullptr && _impl_.crop_ != nullptr) {
    delete _impl_.crop_;
  }
  _impl_.crop_ = nullptr;
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* CropsData::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // string crop_name = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          auto str = _internal_mutable_crop_name();
          ptr = ::_pbi::InlineGreedyStringParser(str, ptr, ctx);
          CHK_(ptr);
          CHK_(::_pbi::VerifyUTF8(str, "editor_serialization.CropsData.crop_name"));
        } else
          goto handle_unusual;
        continue;
      // .editor_serialization.region crop = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 18)) {
          ptr = ctx->ParseMessage(_internal_mutable_crop(), ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* CropsData::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:editor_serialization.CropsData)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // string crop_name = 1;
  if (!this->_internal_crop_name().empty()) {
    ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::VerifyUtf8String(
      this->_internal_crop_name().data(), static_cast<int>(this->_internal_crop_name().length()),
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::SERIALIZE,
      "editor_serialization.CropsData.crop_name");
    target = stream->WriteStringMaybeAliased(
        1, this->_internal_crop_name(), target);
  }

  // .editor_serialization.region crop = 2;
  if (this->_internal_has_crop()) {
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
      InternalWriteMessage(2, _Internal::crop(this),
        _Internal::crop(this).GetCachedSize(), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:editor_serialization.CropsData)
  return target;
}

size_t CropsData::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:editor_serialization.CropsData)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // string crop_name = 1;
  if (!this->_internal_crop_name().empty()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::StringSize(
        this->_internal_crop_name());
  }

  // .editor_serialization.region crop = 2;
  if (this->_internal_has_crop()) {
    total_size += 1 +
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(
        *_impl_.crop_);
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData CropsData::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    CropsData::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*CropsData::GetClassData() const { return &_class_data_; }


void CropsData::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<CropsData*>(&to_msg);
  auto& from = static_cast<const CropsData&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:editor_serialization.CropsData)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (!from._internal_crop_name().empty()) {
    _this->_internal_set_crop_name(from._internal_crop_name());
  }
  if (from._internal_has_crop()) {
    _this->_internal_mutable_crop()->::editor_serialization::region::MergeFrom(
        from._internal_crop());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void CropsData::CopyFrom(const CropsData& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:editor_serialization.CropsData)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool CropsData::IsInitialized() const {
  return true;
}

void CropsData::InternalSwap(CropsData* other) {
  using std::swap;
  auto* lhs_arena = GetArenaForAllocation();
  auto* rhs_arena = other->GetArenaForAllocation();
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr::InternalSwap(
      &_impl_.crop_name_, lhs_arena,
      &other->_impl_.crop_name_, rhs_arena
  );
  swap(_impl_.crop_, other->_impl_.crop_);
}

::PROTOBUF_NAMESPACE_ID::Metadata CropsData::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_editor_5fdata_2eproto_getter, &descriptor_table_editor_5fdata_2eproto_once,
      file_level_metadata_editor_5fdata_2eproto[1]);
}

// ===================================================================

class SerializedCropsData::_Internal {
 public:
};

SerializedCropsData::SerializedCropsData(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:editor_serialization.SerializedCropsData)
}
SerializedCropsData::SerializedCropsData(const SerializedCropsData& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  SerializedCropsData* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.data_){from._impl_.data_}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:editor_serialization.SerializedCropsData)
}

inline void SerializedCropsData::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.data_){arena}
    , /*decltype(_impl_._cached_size_)*/{}
  };
}

SerializedCropsData::~SerializedCropsData() {
  // @@protoc_insertion_point(destructor:editor_serialization.SerializedCropsData)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void SerializedCropsData::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.data_.~RepeatedPtrField();
}

void SerializedCropsData::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void SerializedCropsData::Clear() {
// @@protoc_insertion_point(message_clear_start:editor_serialization.SerializedCropsData)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.data_.Clear();
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* SerializedCropsData::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // repeated .editor_serialization.CropsData data = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 10)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_data(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<10>(ptr));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* SerializedCropsData::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:editor_serialization.SerializedCropsData)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated .editor_serialization.CropsData data = 1;
  for (unsigned i = 0,
      n = static_cast<unsigned>(this->_internal_data_size()); i < n; i++) {
    const auto& repfield = this->_internal_data(i);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
        InternalWriteMessage(1, repfield, repfield.GetCachedSize(), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:editor_serialization.SerializedCropsData)
  return target;
}

size_t SerializedCropsData::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:editor_serialization.SerializedCropsData)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .editor_serialization.CropsData data = 1;
  total_size += 1UL * this->_internal_data_size();
  for (const auto& msg : this->_impl_.data_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData SerializedCropsData::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    SerializedCropsData::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*SerializedCropsData::GetClassData() const { return &_class_data_; }


void SerializedCropsData::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<SerializedCropsData*>(&to_msg);
  auto& from = static_cast<const SerializedCropsData&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:editor_serialization.SerializedCropsData)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  _this->_impl_.data_.MergeFrom(from._impl_.data_);
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void SerializedCropsData::CopyFrom(const SerializedCropsData& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:editor_serialization.SerializedCropsData)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool SerializedCropsData::IsInitialized() const {
  return true;
}

void SerializedCropsData::InternalSwap(SerializedCropsData* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  _impl_.data_.InternalSwap(&other->_impl_.data_);
}

::PROTOBUF_NAMESPACE_ID::Metadata SerializedCropsData::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_editor_5fdata_2eproto_getter, &descriptor_table_editor_5fdata_2eproto_once,
      file_level_metadata_editor_5fdata_2eproto[2]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace editor_serialization
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::editor_serialization::region*
Arena::CreateMaybeMessage< ::editor_serialization::region >(Arena* arena) {
  return Arena::CreateMessageInternal< ::editor_serialization::region >(arena);
}
template<> PROTOBUF_NOINLINE ::editor_serialization::CropsData*
Arena::CreateMaybeMessage< ::editor_serialization::CropsData >(Arena* arena) {
  return Arena::CreateMessageInternal< ::editor_serialization::CropsData >(arena);
}
template<> PROTOBUF_NOINLINE ::editor_serialization::SerializedCropsData*
Arena::CreateMaybeMessage< ::editor_serialization::SerializedCropsData >(Arena* arena) {
  return Arena::CreateMessageInternal< ::editor_serialization::SerializedCropsData >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
