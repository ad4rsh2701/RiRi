#include <ostream>
#include <cstdlib>
#include <string_view>
#include <variant>

#include "doctest.h"
#include "riri/RapidResponse.hpp"
#include "riri/RapidTypes.hpp"
#include "RiRiMacros.h"
#include "riri/utils/Accessors.hpp"

using namespace RiRi::Response;
using namespace RiRi::Utils;


// A small LEGEND, because this is about to get messy
//      1. strv -> string_view (and not the swedish tank series)
//      2. rdt -> rapid data type
//      3. mono -> monostate
//
// =============================================== LISTS OF SUBCASES ===================================================
// +-----------------------------------------------------------------+-------------------------------------------------+
// |                             SUBCASE                             |               Overloads Checked for             |
// +-----------------------------------------------------------------+-------------------------------------------------+
// | 1.  Default status is 'ORPHANED'                                | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |
// | 2.  'begin() == end()' on a default-constructed object          | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |
// | 3.  Setter 'addResultEntry' sets '_overall_code' to 'OK'        | <strv, rdt> :: <rdt, rdt>                       |
// | 4.  Setter 'addStatusEntry' updates/escalates '_overall_code'   | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |
// | 5.  Setter 'addResultEntry' adds a valid entry                  | <strv, rdt> :: <rdt, rdt>                       |
// | 6.  Setter 'addStatusEntry' adds a valid entry                  | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |
// | 7.  Multiple VALID Result Entries; less than static capacity    | <strv, rdt> :: <rdt, rdt>                       |
// | 8.  Multiple VALID Status Entries; less than static capacity    | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |
// | 9.  Multiple Valid Result & Status Entries; less than static    | <strv, rdt> :: <rdt, rdt>                       |
// |     capacity                                                    |                                                 |
// | 10. Status Code Escalation: OK -> WARN -> ERR                   | <strv, rdt> :: <rdt, rdt>                       |
// | 11. Status Code Escalation: OK -> ERR (no WARN step)            | <strv, rdt> :: <rdt, rdt>                       |
// | 12. No de-escalation: ERR status entry followed by result       | <strv, rdt> :: <rdt, rdt>                       |
// |     entry; overall code must remain ERR                         |                                                 |
// | 13. Multiple VALID Result & Status Entries; Upto static         | <strv, rdt> :: <rdt, rdt>                       |
// |     capacity - 1 (entries crosschecked)                         |                                                 |
// | 14. Multiple VALID Result & Status Entries; Exactly at static   | <strv, rdt> :: <rdt, rdt>                       |
// |     capacity (entries crosschecked; boundary)                   |                                                 |
// | 15. Multiple VALID Result & Status Entries; One past static     | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |
// |     capacity (entries crosschecked; first dynamic grow)         |                                                 |
// | 16. Multiple Result & Status Entries; More than first           | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |
// |     dynamically grown capacity (entries crosschecked)           |                                                 |
// | 17. OOM during 'dynamically_grow': setters return early,        | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |
// |     '_entry_count' is frozen, 'code()' returns ERR_OOM          |                                                 |
// | 18. Post-OOM: subsequent 'addResultEntry' and 'addStatusEntry'  | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |
// |     calls are silent no-ops.                                    |                                                 |
// | 19. Iterators cover till entry count; Upto static capacity      | <strv, rdt> :: <rdt, rdt>                       |
// | 20. Iterators cover till entry count; More than static capacity | <strv, rdt> :: <rdt, rdt>                       |
// | 21. Moving 'StatusBatchWith' with static buffer: moved-into     | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |
// |     object is valid, moved-from is reset to empty/ORPHANED      |                                                 |
// | 22. Moving 'StatusBatchWith' with dynamic buffer: moved-into    | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |
// |     object is valid, moved-from is reset to empty/ORPHANED      |                                                 |
// | 23. Moving an empty/ORPHANED 'StatusBatchWith' is safe          | <strv, rdt> :: <strv, mono> :: <rdt, rdt>       |                                                 |
// +-----------------------------------------------------------------+-------------------------------------------------+
// =====================================================================================================================



// ============== FORCE OOM (DO NOT USE) =================

GO_AWAY static bool force_oom = false;

void* operator new(std::size_t size) {
    if (force_oom) throw std::bad_alloc();
    return std::malloc(size);
}

void operator delete(void* ptr) noexcept {
    std::free(ptr);
}

void* operator new[](std::size_t size) {
    if (force_oom) throw std::bad_alloc();
    return std::malloc(size);
}

void operator delete[](void* ptr) noexcept {
    std::free(ptr);
}

    // I wish there was a better approach than this.

// ===========================================


TEST_SUITE("Rapid Response System") {

    TEST_CASE("StatusBatchWith") {

        // response_strv_rdt means the response object is templated with
        // F1 of type std::string_view and F2 of type const RapidDataType*

        // NOTE: F1 cannot be std::monostate (implementation specific)
        // NOTE: I am not in the mood of adding a new specific warning code, so we will use an irrelevant code

        // Data
        RiRi::RapidNode node {"_key", RiRi::RapidDataType("riri")};

        // 1
        SUBCASE("Default status is 'ORPHANED'") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ORPHANED);
            CHECK(response_strv_rdt.ok() == false);

            // <std::string_view, std::monostate>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ORPHANED);
            CHECK(response_strv_mono.ok() == false);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ORPHANED);
            CHECK(response_rdt_rdt.ok() == false);
        }

        // 2
        SUBCASE("'begin() == end()' on a default-constructed object") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            CHECK(response_strv_rdt.ok() == false);
            CHECK(response_strv_rdt.totalEntryCount() == 0);
            CHECK(response_strv_rdt.begin() == response_strv_rdt.end());

            // <std::string_view, std::monostate>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            CHECK(response_strv_mono.ok() == false);
            CHECK(response_strv_mono.totalEntryCount() == 0);
            CHECK(response_strv_mono.begin() == response_strv_mono.end());

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            CHECK(response_rdt_rdt.ok() == false);
            CHECK(response_rdt_rdt.totalEntryCount() == 0);
            CHECK(response_rdt_rdt.begin() == response_rdt_rdt.end());
        }

        // 3
        SUBCASE("Setter 'addResultEntry' sets '_overall_code' to 'OK'") {
            // ESCALATION: ORPHANED -> OK

            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            response_strv_rdt.addResultEntry(node.key, &node.value);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::OK);    // Escalated to OK?
            CHECK(response_strv_rdt.ok() == true);

            // <std::string_view, std::monostate>
            // StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            // response_strv_mono.addResultEntry(node.key, std::monostate{}); <- this will fail.
                // THIS IS NOT ALLOWED
                // as addResultEntry() requires (!std::is_same_v<F2, std::monostate>)

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            response_rdt_rdt.addResultEntry(&node.value, &node.value);  // ehh
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::OK);     // Escalated to OK?
            CHECK(response_rdt_rdt.ok() == true);
        }

        // 4
        SUBCASE("Setter 'addStatusEntry' updates/escalates '_overall_code'") {
            // ESCALATION: ORPHANED -> ERR
                // assumption: Will work for WARN too, so we ain't checking that

            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);    // Should be escalated
            CHECK(response_strv_rdt.ok() == false);

            // <std::string_view, std::monostate>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);   // Should be escalated
            CHECK(response_strv_mono.ok() == false);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);     // Should be escalated
            CHECK(response_rdt_rdt.ok() == false);
        }

        // 5
        SUBCASE("Setter addResultEntry adds a valid entry") {
                // addResultEntry()

            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            response_strv_rdt.addResultEntry(node.key, &node.value);
            CHECK(response_strv_rdt.totalEntryCount() == 1);
            auto it = response_strv_rdt.begin();
            REQUIRE(it != response_strv_rdt.end());
            CHECK(it->target == node.key);
            auto result_strv_rdt = unpack_field(&it->result);
            REQUIRE(result_strv_rdt != nullptr);    // IMP
            CHECK(*result_strv_rdt == node.value);

            // <std::string_view, std::monostate>
                // not applicable

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            response_rdt_rdt.addResultEntry(&node.value, &node.value);
            CHECK(response_rdt_rdt.totalEntryCount() == 1);
            auto it2 = response_rdt_rdt.begin();
            REQUIRE(it2 != response_rdt_rdt.end());
            auto target_rdt_rdt = it2->target;
            REQUIRE(target_rdt_rdt != nullptr);     // IMP
            CHECK(*target_rdt_rdt == node.value);
            auto field_rdt_rdt = unpack_field(&it2->result);
            REQUIRE(field_rdt_rdt != nullptr);      // IMP
            CHECK(*field_rdt_rdt == node.value);
        }

        // 6
        SUBCASE("Setter addStatusEntry adds a valid entry") {
                // addStatusEntry()

            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_strv_rdt.totalEntryCount() == 1);
            auto it = response_strv_rdt.begin();
            REQUIRE(it != response_strv_rdt.end());
            CHECK(it->target == node.key);
            auto code_strv_rdt = unpack_field_code(&it->result);
            REQUIRE(code_strv_rdt != nullptr);      // IMP
            CHECK(*code_strv_rdt == RiRi::StatusCode::ERR_UNKNOWN);

            // <std::string_view, std::monostate>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_strv_mono.totalEntryCount() == 1);
            auto it2 = response_strv_mono.begin();
            REQUIRE(it2 != response_strv_mono.end());
            CHECK(it2->target == node.key);
            auto code_strv_mono = unpack_field_code(&it2->result);
            REQUIRE(code_strv_mono != nullptr);     // IMP
            CHECK(*code_strv_mono == RiRi::StatusCode::ERR_UNKNOWN);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_rdt_rdt.totalEntryCount() == 1);
            auto it3 = response_rdt_rdt.begin();
            REQUIRE(it3 != response_rdt_rdt.end());
            auto target_rdt_rdt = it3->target;
            REQUIRE(target_rdt_rdt != nullptr);     // IMP
            CHECK(*target_rdt_rdt == node.value);
            auto code_rdt_rdt = unpack_field_code(&it3->result);
            REQUIRE(code_rdt_rdt != nullptr);       // IMP
            CHECK(*code_rdt_rdt == RiRi::StatusCode::ERR_UNKNOWN);
        }

        // 7
        SUBCASE("Multiple VALID Result Entries; less than static capacity") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY - 1; ++i) {
                response_strv_rdt.addResultEntry(node.key, &node.value);
            }
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::OK);
            CHECK(response_strv_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY - 1);
            REQUIRE(response_strv_rdt.begin() != response_strv_rdt.end());
            for (const auto&[target, field] : response_strv_rdt) {
                CHECK(target == node.key);
                auto field_rdt = unpack_field(&field);
                REQUIRE(field_rdt != nullptr);     // IMP
                CHECK(*field_rdt == node.value);
            }

            // <std::string_view, std::monostate>
                // not applicable

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY - 1; ++i) {
                response_rdt_rdt.addResultEntry(&node.value, &node.value);
            }
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::OK);
            CHECK(response_rdt_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY - 1);
            REQUIRE(response_rdt_rdt.begin() != response_rdt_rdt.end());
            for (const auto&[target, field] : response_rdt_rdt) {
                REQUIRE(target != nullptr);     // IMP
                CHECK(*target == node.value);
                auto field_rdt = unpack_field(&field);
                REQUIRE(field_rdt != nullptr);     // IMP
                CHECK(*field_rdt == node.value);
            }
        }

        // 8
        SUBCASE("Multiple VALID Status Entries; less than static capacity") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY - 1; ++i) {
                response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_strv_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY - 1);
            REQUIRE(response_strv_rdt.begin() != response_strv_rdt.end());
            for (const auto&[target, field] : response_strv_rdt) {
                CHECK(target == node.key);
                auto field_code = unpack_field_code(&field);
                REQUIRE(field_code != nullptr);      // IMP
                CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
            }

            // <std::string_view, std::monostate>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY - 1; ++i) {
                response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_strv_mono.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY - 1);
            REQUIRE(response_strv_mono.begin() != response_strv_mono.end());
            for (const auto&[target, field] : response_strv_mono) {
                CHECK(target == node.key);
                auto field_code = unpack_field_code(&field);
                REQUIRE(field_code != nullptr);      // IMP
                CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
            }

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY - 1; ++i) {
                response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_rdt_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY - 1);
            REQUIRE(response_rdt_rdt.begin() != response_rdt_rdt.end());
            for (const auto&[target, field] : response_rdt_rdt) {
                REQUIRE(target != nullptr);         // IMP
                CHECK(*target == node.value);
                auto field_code = unpack_field_code(&field);
                REQUIRE(field_code != nullptr);       // IMP
                CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
            }
        }

        // 9
        SUBCASE("Multiple Valid Result & Status Entries; less than static capacity") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY/2 - 1; ++i) {
                response_strv_rdt.addResultEntry(node.key, &node.value);
            }
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::OK);
            CHECK(response_strv_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY/2 - 1);
            REQUIRE(response_strv_rdt.begin() != response_strv_rdt.end());
            for (const auto&[target, field] : response_strv_rdt) {
                CHECK(target == node.key);
                auto field_rdt = unpack_field(&field);
                REQUIRE(field_rdt != nullptr);      // IMP
                CHECK(*field_rdt == node.value);
            }
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY/2 - 1; ++i) {
                response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_strv_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY - 2); // ((x/2)-1)+((x/2)-1)=x-2
            auto it_strv_rdt = response_strv_rdt.begin()+(TARGET_FIELD_TRACKING_CAPACITY/2-1);
            REQUIRE(it_strv_rdt != response_strv_rdt.end());
            for (; it_strv_rdt != response_strv_rdt.end(); ++it_strv_rdt) {
                const auto& [target, field] = *it_strv_rdt;
                CHECK(target == node.key);
                auto field_code = unpack_field_code(&field);
                REQUIRE(field_code != nullptr);      // IMP
                CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
            }

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY/2 - 1; ++i) {
                response_rdt_rdt.addResultEntry(&node.value, &node.value);
            }
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::OK);
            CHECK(response_rdt_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY/2 - 1);
            REQUIRE(response_rdt_rdt.begin() != response_rdt_rdt.end());
            for (const auto&[target, field] : response_rdt_rdt) {
                REQUIRE(target != nullptr);         // IMP
                CHECK(*target == node.value);
                auto field_rdt = unpack_field(&field);
                REQUIRE(field_rdt != nullptr);      // IMP
                CHECK(*field_rdt == node.value);
            }
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY/2 - 1; i++) {
                response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_rdt_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY - 2);
            auto it_rdt_rdt = response_rdt_rdt.begin()+(TARGET_FIELD_TRACKING_CAPACITY/2-1);
            REQUIRE(it_rdt_rdt != response_rdt_rdt.end());
            for (; it_rdt_rdt != response_rdt_rdt.end(); ++it_rdt_rdt) {
                const auto& [target, field] = *it_rdt_rdt;
                REQUIRE(target != nullptr);         // IMP
                CHECK(*target == node.value);
                auto field_code = unpack_field_code(&field);
                REQUIRE(field_code != nullptr);       // IMP
                CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
            }
        }

        // 10
        SUBCASE("Status Code Escalation: OK -> WARN -> ERR") {
            // ESCALATION: ORPHANED -> OK -> WARN -> ERR
                // satisfies: OK->WARN, WARN->ERR

            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            response_strv_rdt.addResultEntry(node.key, &node.value);                             // Escalate to OK
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::OK);
            response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::WARN_KEY_STORE_NEARING_CAPACITY);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::WARN_RESPONSE_CONTAINS_WARNINGS);   // Should escalate to WARN_
            CHECK(response_strv_rdt.ok() == false);
            response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);        // Should escalate to ERR_
            CHECK(response_strv_rdt.ok() == false);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            response_rdt_rdt.addResultEntry(&node.value, &node.value);            // Escalate to OK
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::OK);
            response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::WARN_KEY_STORE_NEARING_CAPACITY);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::WARN_RESPONSE_CONTAINS_WARNINGS);    // Should escalate to WARN_
            CHECK(response_rdt_rdt.ok() == false);
            response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);         // Should escalate to ERR_
            CHECK(response_rdt_rdt.ok() == false);
        }

        // 11
        SUBCASE("Status Code Escalation: OK -> ERR") {
            // ESCALATION: ORPHANED -> OK -> ERR
                // satisfies: OK->ERR

            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            response_strv_rdt.addResultEntry(node.key, &node.value);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::OK);                                // Escalate to OK
            response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);        // Should escalate to ERR_
            CHECK(response_strv_rdt.ok() == false);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            response_rdt_rdt.addResultEntry(&node.value, &node.value);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::OK);                                 // Escalate to OK
            response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);         // Should escalate to ERR_
            CHECK(response_rdt_rdt.ok() == false);
        }

        // 12
        SUBCASE("No de-escalation: ERR status entry followed by result entry; overall code must remain ERR") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            response_strv_rdt.addResultEntry(node.key, &node.value);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::WARN_KEY_STORE_NEARING_CAPACITY);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_strv_rdt.totalEntryCount() == 3);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            response_rdt_rdt.addResultEntry(&node.value, &node.value);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::WARN_KEY_STORE_NEARING_CAPACITY);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_rdt_rdt.totalEntryCount() == 3);
        }

        // 13
        SUBCASE("Multiple VALID Result & Status Entries; Upto static capacity - 1 (entries crosschecked)") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY/2 - 1; ++i) {
                response_strv_rdt.addResultEntry(node.key, &node.value);
                response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            response_strv_rdt.addResultEntry(node.key, &node.value); // one extra entry to keep it 1 less than max
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_strv_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY-1);
            REQUIRE(response_strv_rdt.begin() != response_strv_rdt.end());
            int rdt_count = 0, status_count = 0;
            for (const auto&[target, field] : response_strv_rdt) {
                CHECK(target == node.key);
                if (auto field_rdt = unpack_field(&field)) {
                    CHECK(*field_rdt == node.value);
                    rdt_count++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count++;
                }
            }
            CHECK(rdt_count == TARGET_FIELD_TRACKING_CAPACITY / 2);  // TARGET_FIELD_TRACKING_CAPACITY/2 - 1 + 1 extra
            CHECK(status_count == TARGET_FIELD_TRACKING_CAPACITY / 2 - 1);
            CHECK(rdt_count + status_count == TARGET_FIELD_TRACKING_CAPACITY - 1);


            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0 ; i < TARGET_FIELD_TRACKING_CAPACITY / 2 - 1; i++) {
                response_rdt_rdt.addResultEntry(&node.value, &node.value);
                response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
            response_rdt_rdt.addResultEntry(&node.value, &node.value);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            CHECK(response_rdt_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY - 1);
            REQUIRE(response_rdt_rdt.begin() != response_rdt_rdt.end());
            int rdt_count2 = 0, status_count2 = 0;
            for (const auto&[target, field] : response_rdt_rdt) {
                REQUIRE(target != nullptr);
                CHECK(*target == node.value);
                if (auto field_rdt = unpack_field(&field)) {
                    CHECK(*field_rdt == node.value);
                    rdt_count2++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count2++;
                }
            }
            CHECK(rdt_count2 == TARGET_FIELD_TRACKING_CAPACITY / 2);
            CHECK(status_count2 == TARGET_FIELD_TRACKING_CAPACITY / 2 - 1);
            CHECK(rdt_count2 + status_count2 == TARGET_FIELD_TRACKING_CAPACITY - 1);
        }

        // 14
        SUBCASE("Multiple VALID Result & Status Entries; Exactly at static capacity (entries crosschecked; boundary)") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_strv_rdt.addResultEntry(node.key, &node.value);
                response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            CHECK(response_strv_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_strv_rdt.begin() != response_strv_rdt.end());
            int rdt_count = 0, status_count = 0;
            for (const auto&[target, field] : response_strv_rdt) {
                CHECK(target == node.key);
                if (auto field_rdt = unpack_field(&field)) {
                    CHECK(*field_rdt == node.value);
                    rdt_count++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count++;
                }
            }
            CHECK(rdt_count == TARGET_FIELD_TRACKING_CAPACITY / 2);
            CHECK(status_count == TARGET_FIELD_TRACKING_CAPACITY / 2);
            CHECK(rdt_count + status_count == TARGET_FIELD_TRACKING_CAPACITY);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_rdt_rdt.addResultEntry(&node.value, &node.value);
                response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
            CHECK(response_rdt_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_rdt_rdt.begin() != response_rdt_rdt.end());
            int rdt_count2 = 0, status_count2 = 0;
            for (const auto&[target, field] : response_rdt_rdt) {
                REQUIRE(target != nullptr);
                CHECK(*target == node.value);
                if (auto field_rdt = unpack_field(&field)) {
                    CHECK(*field_rdt == node.value);
                    rdt_count2++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count2++;
                }
            }
            CHECK(rdt_count2 == TARGET_FIELD_TRACKING_CAPACITY / 2);
            CHECK(status_count2 == TARGET_FIELD_TRACKING_CAPACITY / 2);
            CHECK(rdt_count2 + status_count2 == TARGET_FIELD_TRACKING_CAPACITY);
        }

        // 15
        SUBCASE("Multiple VALID Result & Status Entries; One past static capacity (entries crosschecked; first dynamic grow)") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_strv_rdt.addResultEntry(node.key, &node.value);
                response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            response_strv_rdt.addResultEntry(node.key, &node.value);    // 1 past static capacity
            CHECK(response_strv_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY+1);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_strv_rdt.begin() != response_strv_rdt.end());
            int rdt_count = 0, status_count = 0;
            for (const auto&[target, field] : response_strv_rdt) {
                CHECK(target == node.key);
                if (auto field_rdt = unpack_field(&field)) {
                    CHECK(*field_rdt == node.value);
                    rdt_count++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count++;
                }
            }
            CHECK(rdt_count == TARGET_FIELD_TRACKING_CAPACITY / 2 + 1);
            CHECK(status_count == TARGET_FIELD_TRACKING_CAPACITY / 2);
            CHECK(rdt_count + status_count == TARGET_FIELD_TRACKING_CAPACITY + 1);

            // <std::string_view, std::mono>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY; ++i) {
                response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN); // 1 past static capacity
            CHECK(response_strv_mono.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY + 1);
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_strv_mono.begin() != response_strv_mono.end());
            int status_count2 = 0;
            for (const auto&[target, field] : response_strv_mono) {
                CHECK(target == node.key);
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count2++;
                }
            }
            CHECK(status_count2 == TARGET_FIELD_TRACKING_CAPACITY + 1);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_rdt_rdt.addResultEntry(&node.value, &node.value);
                response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
            response_rdt_rdt.addResultEntry(&node.value, &node.value);  // 1 past static capacity
            CHECK(response_rdt_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY + 1);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_rdt_rdt.begin() != response_rdt_rdt.end());
            int rdt_count3 = 0, status_count3 = 0;
            for (const auto&[target, field] : response_rdt_rdt) {
                REQUIRE(target != nullptr);
                CHECK(*target == node.value);
                if (auto field_rdt = unpack_field(&field)) {
                    CHECK(*field_rdt == node.value);
                    rdt_count3++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count3++;
                }
            }
            CHECK(rdt_count3 == TARGET_FIELD_TRACKING_CAPACITY / 2 + 1);
            CHECK(status_count3 == TARGET_FIELD_TRACKING_CAPACITY / 2);
            CHECK(rdt_count3 + status_count3 == TARGET_FIELD_TRACKING_CAPACITY + 1);
        }

        // 16
        SUBCASE("Multiple VALID Result & Status Entries; More than first dynamically grown capacity (entries crosschecked)") {
            // rahhh
            auto one_less_than_entry_count_to_trigger_growth_again = (3*TARGET_FIELD_TRACKING_CAPACITY+16)/2;
                // y > x + x/2 + 8: must be greater than our growth rate
                // y > 3x/2 + 16/2
                // y > (3x+16)/2

            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < one_less_than_entry_count_to_trigger_growth_again / 2; ++i) {
                response_strv_rdt.addResultEntry(node.key, &node.value);
                response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            response_strv_rdt.addResultEntry(node.key, &node.value); // one more to trigger growth again
            CHECK(response_strv_rdt.totalEntryCount() == one_less_than_entry_count_to_trigger_growth_again + 1);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_strv_rdt.begin() != response_strv_rdt.end());
            int rdt_count = 0, status_count = 0;
            for (const auto&[target, field] : response_strv_rdt) {
                CHECK(target == node.key);
                if (auto field_rdt = unpack_field(&field)) {
                    CHECK(*field_rdt == node.value);
                    rdt_count++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count++;
                }
            }
            CHECK(rdt_count == one_less_than_entry_count_to_trigger_growth_again / 2 + 1);
            CHECK(status_count == one_less_than_entry_count_to_trigger_growth_again / 2);
            CHECK(rdt_count + status_count == one_less_than_entry_count_to_trigger_growth_again + 1);

            // <std::string_view, std::monostate>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            for (int i = 0; i < one_less_than_entry_count_to_trigger_growth_again; ++i) {
                response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN); // one more to trigger growth
            CHECK(response_strv_mono.totalEntryCount() == one_less_than_entry_count_to_trigger_growth_again + 1);
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_strv_mono.begin() != response_strv_mono.end());
            int status_count2 = 0;
            for (const auto&[target, field] : response_strv_mono) {
                CHECK(target == node.key);
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count2++;
                }
            }
            CHECK(status_count2 == one_less_than_entry_count_to_trigger_growth_again + 1);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < one_less_than_entry_count_to_trigger_growth_again/2; i++) {
                response_rdt_rdt.addResultEntry(&node.value, &node.value);
                response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
            response_rdt_rdt.addResultEntry(&node.value, &node.value);
            CHECK(response_rdt_rdt.totalEntryCount() == one_less_than_entry_count_to_trigger_growth_again + 1);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_rdt_rdt.begin() != response_rdt_rdt.end());
            int rdt_count3 = 0, status_count3 = 0;
            for (const auto&[target, field] : response_rdt_rdt) {
                REQUIRE(target != nullptr);
                CHECK(*target == node.value);
                if (auto field_code = unpack_field(&field)) {
                    CHECK(*field_code == node.value);
                    rdt_count3++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count3++;
                }
            }
            CHECK(rdt_count3 == one_less_than_entry_count_to_trigger_growth_again / 2 + 1);
            CHECK(status_count3 == one_less_than_entry_count_to_trigger_growth_again / 2);
            CHECK(rdt_count3 + status_count3 == one_less_than_entry_count_to_trigger_growth_again + 1);
        }

        /////////////////////// FORCED OOM ZONE ///////////////////////
        // 17
        SUBCASE("OOM during 'dynamically_grow': setters return early, '_entry_count' is frozen, 'code()' returns ERR_OOM") {
            // Data
            RiRi::RapidNode rapid_node{"_key", RiRi::RapidDataType("riri")};

            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_strv_rdt.addResultEntry(rapid_node.key, &rapid_node.value);
                response_strv_rdt.addStatusEntry(rapid_node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            force_oom = true;
            response_strv_rdt.addResultEntry(rapid_node.key, &rapid_node.value);
            force_oom = false;
            CHECK(response_strv_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY); // frozen at 8
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_OUT_OF_MEMORY);

            // <std::string_view, std::monostate>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY; ++i) {
                response_strv_mono.addStatusEntry(rapid_node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            force_oom = true;
            response_strv_mono.addStatusEntry(rapid_node.key, RiRi::StatusCode::ERR_UNKNOWN);
            force_oom = false;
            CHECK(response_strv_mono.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ERR_OUT_OF_MEMORY);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_rdt_rdt.addResultEntry(&rapid_node.value, &rapid_node.value);
                response_rdt_rdt.addStatusEntry(&rapid_node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
            force_oom = true;
            response_rdt_rdt.addResultEntry(&rapid_node.value, &rapid_node.value);
            force_oom = false;
            CHECK(response_rdt_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_OUT_OF_MEMORY);
        }

        // 18
        SUBCASE("Post-OOM: subsequent 'addResultEntry' and 'addStatusEntry' calls are silent no-ops") {
            // Data
            RiRi::RapidNode rapid_node {"_key", RiRi::RapidDataType("riri")};

            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_strv_rdt.addResultEntry(node.key, &node.value);
                response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            force_oom = true;
            response_strv_rdt.addResultEntry(node.key, &node.value);
            force_oom = false;
            response_strv_rdt.addResultEntry(node.key, &node.value);
            response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_strv_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_OUT_OF_MEMORY);

            // <std::string_view, std::monostate>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY; ++i) {
                response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            force_oom = true;
            response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            force_oom = false;
            response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::WARN_KEY_STORE_NEARING_CAPACITY); // ehh
                // addResultEntry() is not valid for this overload
            CHECK(response_strv_mono.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ERR_OUT_OF_MEMORY);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_rdt_rdt.addResultEntry(&node.value, &node.value);
                response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
            force_oom = true;
            response_rdt_rdt.addResultEntry(&node.value, &node.value);
            force_oom = false;
            response_rdt_rdt.addResultEntry(&node.value, &node.value);
            response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            CHECK(response_rdt_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_OUT_OF_MEMORY);
        }
        ///////////////////////////////////////////////////////////////

        // 19
        SUBCASE("Iterators cover till entry count; Upto static capacity") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_strv_rdt.addResultEntry(node.key, &node.value);
                response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            REQUIRE(response_strv_rdt.begin() != response_strv_rdt.end());
            int count = 0;
            for (const auto& _ : response_strv_rdt) { ++count; }
            CHECK(count == response_strv_rdt.totalEntryCount());
            CHECK(count == TARGET_FIELD_TRACKING_CAPACITY);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_rdt_rdt.addResultEntry(&node.value, &node.value);
                response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
            REQUIRE(response_rdt_rdt.begin() != response_rdt_rdt.end());
            int count2 = 0;
            for (const auto& _ : response_rdt_rdt) { ++count2; }
            CHECK(count2 == response_rdt_rdt.totalEntryCount());
            CHECK(count2 == TARGET_FIELD_TRACKING_CAPACITY);
        }

        // 20
        SUBCASE("Iterators cover till entry count; More than static capacity") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_strv_rdt.addResultEntry(node.key, &node.value);
                response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
                // adding two more to go beyond static capacity
            response_strv_rdt.addResultEntry(node.key, &node.value);
            response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            REQUIRE(response_strv_rdt.begin() != response_strv_rdt.end());
            int count = 0;
            for (const auto& _ : response_strv_rdt) { ++count; }
            CHECK(count == response_strv_rdt.totalEntryCount());
            CHECK(count == TARGET_FIELD_TRACKING_CAPACITY + 2);

            // <const RapidDataType*, const RapidDataTpe*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_rdt_rdt.addResultEntry(&node.value, &node.value);
                response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
                // adding to go beyond static capacity
            response_rdt_rdt.addResultEntry(&node.value, &node.value);
            response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            REQUIRE(response_rdt_rdt.begin() != response_rdt_rdt.end());
            int count2 = 0;
            for (const auto& _ : response_rdt_rdt) { ++count2; }
            CHECK(count2 == response_rdt_rdt.totalEntryCount());
            CHECK(count2 == TARGET_FIELD_TRACKING_CAPACITY + 2);
        }

        // 21
        SUBCASE("Moving 'StatusBatchWith' with static buffer: moved-into object is valid, moved-from is reset to empty/ORPHANED") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_strv_rdt.addResultEntry(node.key, &node.value);
                response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            REQUIRE(response_strv_rdt.begin() != response_strv_rdt.end());
            CHECK(response_strv_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            StatusBatchWith response_strv_rdt_moved(std::move(response_strv_rdt));          // MOVE
            REQUIRE(response_strv_rdt_moved.begin() != response_strv_rdt_moved.end());
            CHECK(response_strv_rdt_moved.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY); // ENTRY COUNT CHECK
            CHECK(response_strv_rdt_moved.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);  // CODE CHECK
            REQUIRE(response_strv_rdt.begin() == response_strv_rdt.end());  // POST-MOVE old object iterators reset
            CHECK(response_strv_rdt.totalEntryCount() == 0);    // POST-MOVE old object count reset
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ORPHANED);  // POST-MOVE old object code reset
            int rdt_count = 0, status_count = 0;
            for (const auto& [target, field] : response_strv_rdt_moved) {
                CHECK(target == node.key);
                if (auto field_rdt = unpack_field(&field)) {
                    CHECK(*field_rdt == node.value);
                    rdt_count++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count++;
                }
            }
            CHECK(rdt_count == TARGET_FIELD_TRACKING_CAPACITY / 2);
            CHECK(status_count == TARGET_FIELD_TRACKING_CAPACITY / 2);
            CHECK(rdt_count + status_count == TARGET_FIELD_TRACKING_CAPACITY);
            int count = 0;
            for (const auto& _ : response_strv_rdt) { ++count; }
            CHECK(count == 0);


            // <std::string_view, std::monostate>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY; ++i) {
                response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
            REQUIRE(response_strv_mono.begin() != response_strv_mono.end());
            CHECK(response_strv_mono.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            StatusBatchWith response_strv_mono_moved(std::move(response_strv_mono));        // MOVE
            REQUIRE(response_strv_mono_moved.begin() != response_strv_mono_moved.end());
            CHECK(response_strv_mono_moved.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_strv_mono_moved.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_strv_mono.begin() == response_strv_mono.end());
            CHECK(response_strv_mono.totalEntryCount() == 0);
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ORPHANED);
            int status_count2 = 0;
            for (const auto& [target, field] : response_strv_mono_moved) {
                CHECK(target == node.key);
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count2++;
                }
            }
            CHECK(status_count2 == TARGET_FIELD_TRACKING_CAPACITY);
            int count2 = 0;
            for (const auto& _ : response_strv_mono) { ++count2; }
            CHECK(count2 == 0);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_rdt_rdt.addResultEntry(&node.value, &node.value);
                response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
            REQUIRE(response_rdt_rdt.begin() != response_rdt_rdt.end());
            CHECK(response_rdt_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            StatusBatchWith response_rdt_rdt_moved(std::move(response_rdt_rdt));        // MOVE
            REQUIRE(response_rdt_rdt_moved.begin() != response_rdt_rdt_moved.end());
            CHECK(response_rdt_rdt_moved.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY);
            CHECK(response_rdt_rdt_moved.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_rdt_rdt.begin() == response_rdt_rdt.end());
            CHECK(response_rdt_rdt.totalEntryCount() == 0);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ORPHANED);
            int rdt_count3 = 0, status_count3 = 0;
                // verifying if the entries actually exist in the new object
            for (const auto& [target, field] : response_rdt_rdt_moved) {
                REQUIRE(target != nullptr);
                CHECK(*target == node.value);
                if (auto field_rdt = unpack_field(&field)) {
                    CHECK(*field_rdt == node.value);
                    rdt_count3++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count3++;
                }
            }
            CHECK(rdt_count3 == TARGET_FIELD_TRACKING_CAPACITY / 2);
            CHECK(status_count3 == TARGET_FIELD_TRACKING_CAPACITY / 2);
            CHECK(rdt_count3 + status_count3 == TARGET_FIELD_TRACKING_CAPACITY);
                // verifying the absence of entries in the old object post-move
            int count3 = 0;
            for (const auto& _ : response_rdt_rdt) { ++count3; }
            CHECK(count3 == 0);
        }

        // 22
        SUBCASE("Moving 'StatusBatchWith' with dynamic buffer: moved-into object is valid, moved-from is reset to empty/ORPHANED") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_strv_rdt.addResultEntry(node.key, &node.value);
                response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
                // adding two more to trigger dynamic growth
            response_strv_rdt.addResultEntry(node.key, &node.value);
            response_strv_rdt.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            REQUIRE(response_strv_rdt.begin() != response_strv_rdt.end());
            CHECK(response_strv_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY+2);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            StatusBatchWith response_strv_rdt_moved(std::move(response_strv_rdt));      // MOVE
            REQUIRE(response_strv_rdt_moved.begin() != response_strv_rdt_moved.end());
            CHECK(response_strv_rdt_moved.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY+2);   // ENTRY COUNT CHECK
            CHECK(response_strv_rdt_moved.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);  // CODE CHECK
            REQUIRE(response_strv_rdt.begin() == response_strv_rdt.end());  // POST-MOVE old object iterators reset
            CHECK(response_strv_rdt.totalEntryCount() == 0);    // POST-MOVE old object count reset
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ORPHANED);  // POST-MOVE old object status reset
                // verifying if the entries actually exist
            int rdt_count = 0, status_count = 0;
            for (const auto& [target, field] : response_strv_rdt_moved) {
                CHECK(target == node.key);
                if (auto field_rdt = unpack_field(&field)) {
                    CHECK(*field_rdt == node.value);
                    rdt_count++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count++;
                }
            }
            CHECK(rdt_count == TARGET_FIELD_TRACKING_CAPACITY / 2 + 1);
            CHECK(status_count == TARGET_FIELD_TRACKING_CAPACITY / 2 + 1);
            CHECK(rdt_count + status_count == TARGET_FIELD_TRACKING_CAPACITY + 2);
                // verifying the absence of entries in the old object POST MOVE
            int count = 0;
            for (const auto& _ : response_strv_rdt) { ++count; }
            CHECK(count == 0);

            // <std::string_view, std::mono>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY; ++i) {
                response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            }
                // adding one more to trigger dynamic growth
            response_strv_mono.addStatusEntry(node.key, RiRi::StatusCode::ERR_UNKNOWN);
            REQUIRE(response_strv_mono.begin() != response_strv_mono.end());
            CHECK(response_strv_mono.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY + 1);
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            StatusBatchWith response_strv_mono_moved(std::move(response_strv_mono));        // MOVE
            REQUIRE(response_strv_mono_moved.begin() != response_strv_mono_moved.end());
            CHECK(response_strv_mono_moved.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY + 1);
            CHECK(response_strv_mono_moved.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_strv_mono.begin() == response_strv_mono.end());
            CHECK(response_strv_mono.totalEntryCount() == 0);
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ORPHANED);
            int status_count2 = 0;
            for (const auto& [target, field] : response_strv_mono_moved) {
                CHECK(target == node.key);
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count2++;
                }
            }
            CHECK(status_count2 == TARGET_FIELD_TRACKING_CAPACITY + 1);
            int count2 = 0;
            for (const auto& _ : response_strv_mono) { ++count2; }
            CHECK(count2 == 0);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            for (int i = 0; i < TARGET_FIELD_TRACKING_CAPACITY / 2; ++i) {
                response_rdt_rdt.addResultEntry(&node.value, &node.value);
                response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            }
                // adding two more to trigger dynamic growth
            response_rdt_rdt.addResultEntry(&node.value, &node.value);
            response_rdt_rdt.addStatusEntry(&node.value, RiRi::StatusCode::ERR_UNKNOWN);
            REQUIRE(response_rdt_rdt.begin() != response_rdt_rdt.end());
            CHECK(response_rdt_rdt.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY + 2);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            StatusBatchWith response_rdt_rdt_moved(std::move(response_rdt_rdt));    // MOVE
            REQUIRE(response_rdt_rdt_moved.begin() != response_rdt_rdt_moved.end());
            CHECK(response_rdt_rdt_moved.totalEntryCount() == TARGET_FIELD_TRACKING_CAPACITY + 2);
            CHECK(response_rdt_rdt_moved.code() == RiRi::StatusCode::ERR_SOME_OPERATIONS_FAILED);
            REQUIRE(response_rdt_rdt.begin() == response_rdt_rdt.end());
            CHECK(response_rdt_rdt.totalEntryCount() == 0);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ORPHANED);
            int rdt_count3 = 0, status_count3 = 0;
            for (const auto&[target, field]: response_rdt_rdt_moved) {
                REQUIRE(target != nullptr);
                CHECK(*target == node.value);
                if (auto field_rdt = unpack_field(&field)) {
                    CHECK(*field_rdt == node.value);
                    rdt_count3++;
                }
                if (auto field_code = unpack_field_code(&field)) {
                    CHECK(*field_code == RiRi::StatusCode::ERR_UNKNOWN);
                    status_count3++;
                }
            }
            CHECK(rdt_count3 == TARGET_FIELD_TRACKING_CAPACITY / 2 + 1);
            CHECK(status_count3 == TARGET_FIELD_TRACKING_CAPACITY / 2 + 1);
            CHECK(rdt_count3 + status_count3 == TARGET_FIELD_TRACKING_CAPACITY + 2);
            int count3 = 0;
            for (const auto& _ : response_rdt_rdt) { ++count3; }
            CHECK(count3 == 0);
        }

        // 23
        SUBCASE("Moving an empty/ORPHANED 'StatusBatchWith' is safe") {
            // <std::string_view, const RapidDataType*>
            StatusBatchWith<std::string_view, const RiRi::RapidDataType*> response_strv_rdt;
            REQUIRE(response_strv_rdt.begin() == response_strv_rdt.end());
            CHECK(response_strv_rdt.totalEntryCount() == 0);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ORPHANED);
                // MOVE
            StatusBatchWith response_strv_rdt_moved(std::move(response_strv_rdt));
            REQUIRE(response_strv_rdt_moved.begin() == response_strv_rdt_moved.end());
            CHECK(response_strv_rdt_moved.totalEntryCount() == 0);
            CHECK(response_strv_rdt_moved.code() == RiRi::StatusCode::ORPHANED);
                // Checking the old object
            REQUIRE(response_strv_rdt.begin() == response_strv_rdt.end());
            CHECK(response_strv_rdt.totalEntryCount() == 0);
            CHECK(response_strv_rdt.code() == RiRi::StatusCode::ORPHANED);

            // <std::string_view, std::monostate>
            StatusBatchWith<std::string_view, std::monostate> response_strv_mono;
            REQUIRE(response_strv_mono.begin() == response_strv_mono.end());
            CHECK(response_strv_mono.totalEntryCount() == 0);
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ORPHANED);
                // MOVE
            StatusBatchWith response_strv_mono_moved(std::move(response_strv_mono));
            REQUIRE(response_strv_mono_moved.begin() == response_strv_mono_moved.end());
            CHECK(response_strv_mono_moved.totalEntryCount() == 0);
            CHECK(response_strv_mono_moved.code() == RiRi::StatusCode::ORPHANED);
                // Checking the old object
            REQUIRE(response_strv_mono.begin() == response_strv_mono.end());
            CHECK(response_strv_mono.totalEntryCount() == 0);
            CHECK(response_strv_mono.code() == RiRi::StatusCode::ORPHANED);

            // <const RapidDataType*, const RapidDataType*>
            StatusBatchWith<const RiRi::RapidDataType*, const RiRi::RapidDataType*> response_rdt_rdt;
            REQUIRE(response_rdt_rdt.begin() == response_rdt_rdt.end());
            CHECK(response_rdt_rdt.totalEntryCount() == 0);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ORPHANED);
                // MOVE
            StatusBatchWith response_rdt_rdt_moved(std::move(response_rdt_rdt));
            REQUIRE(response_rdt_rdt_moved.begin() == response_rdt_rdt_moved.end());
            CHECK(response_rdt_rdt_moved.totalEntryCount() == 0);
            CHECK(response_rdt_rdt_moved.code() == RiRi::StatusCode::ORPHANED);
                // Checking old object
            REQUIRE(response_rdt_rdt.begin() == response_rdt_rdt.end());
            CHECK(response_rdt_rdt.totalEntryCount() == 0);
            CHECK(response_rdt_rdt.code() == RiRi::StatusCode::ORPHANED);
        }

    }
}