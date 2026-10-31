#include <fstream>
#include <string>
#include <string_view>

namespace {
    // DirectInput scancode for the F8 key. Change this to rebind the export hotkey.
    // (F5 = quicksave, F9 = quickload are taken by vanilla; F8 is free by default.)
    constexpr std::uint32_t kExportKey = 0x42;

    constexpr auto kOutputFileName = "IngredientExporter_Inventory.json";

    // Minimal JSON string escaping for ingredient / effect names.
    std::string JsonEscape(std::string_view text) {
        std::string out;
        out.reserve(text.size() + 2);
        for (const char c : text) {
            switch (c) {
                case '"':  out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\n': out += "\\n";  break;
                case '\r': out += "\\r";  break;
                case '\t': out += "\\t";  break;
                default:   out += c;      break;
            }
        }
        return out;
    }

    // Walk the player's inventory, keep the alchemy ingredients, and write each one's
    // name, count, and effect names out as JSON next to the SKSE log.
    void ExportIngredients() {
        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) {
            return;
        }

        const auto logDir = SKSE::log::log_directory();
        if (!logDir) {
            SKSE::log::error("Could not resolve the SKSE log directory; export aborted");
            return;
        }
        const auto outputPath = *logDir / kOutputFileName;

        std::ofstream file(outputPath);
        if (!file) {
            SKSE::log::error("Failed to open {} for writing", outputPath.string());
            return;
        }

        const auto inventory = player->GetInventory([](RE::TESBoundObject& obj) {
            return obj.Is(RE::FormType::Ingredient);
        });

        std::size_t written = 0;
        bool firstItem = true;
        file << "{\n  \"ingredients\": [\n";
        for (const auto& [obj, entry] : inventory) {
            const std::int32_t count = entry.first;
            if (count <= 0) {
                continue;  // skip anything not actually carried
            }

            // Filtered on FormType::Ingredient above, so this cast is safe.
            auto* ingredient = static_cast<RE::IngredientItem*>(obj);
            const char* rawName = ingredient->GetName();
            const std::string name = (rawName && rawName[0]) ? rawName : "<unnamed>";

            if (!firstItem) {
                file << ",\n";
            }
            firstItem = false;
            ++written;

            file << "    { \"name\": \"" << JsonEscape(name) << "\", \"count\": " << count
                 << ", \"effects\": [";
            bool firstEffect = true;
            for (auto* effect : ingredient->effects) {
                if (!effect || !effect->baseEffect) {
                    continue;
                }
                const char* effectName = effect->baseEffect->GetName();
                if (!effectName || !effectName[0]) {
                    continue;
                }
                if (!firstEffect) {
                    file << ", ";
                }
                firstEffect = false;
                file << '"' << JsonEscape(effectName) << '"';
            }
            file << "] }";
        }
        file << "\n  ]\n}\n";

        SKSE::log::info("Exported {} ingredient(s) to {}", written, outputPath.string());
        if (auto* console = RE::ConsoleLog::GetSingleton()) {
            console->Print("IngredientExporter: wrote %llu ingredient(s) to %s",
                           static_cast<unsigned long long>(written), outputPath.string().c_str());
        }
    }

    // Listens for the export hotkey while in-game.
    class InputSink : public RE::BSTEventSink<RE::InputEvent*> {
    public:
        static InputSink* GetSingleton() {
            static InputSink singleton;
            return &singleton;
        }

        RE::BSEventNotifyControl ProcessEvent(RE::InputEvent* const* a_event,
                                              RE::BSTEventSource<RE::InputEvent*>*) override {
            if (!a_event) {
                return RE::BSEventNotifyControl::kContinue;
            }
            for (auto* event = *a_event; event; event = event->next) {
                auto* button = event->AsButtonEvent();
                if (!button || !button->IsDown()) {
                    continue;
                }
                if (button->GetDevice() == RE::INPUT_DEVICE::kKeyboard &&
                    button->GetIDCode() == kExportKey) {
                    ExportIngredients();
                }
            }
            return RE::BSEventNotifyControl::kContinue;
        }
    };
}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {
    SKSE::Init(skse);

    SKSE::GetMessagingInterface()->RegisterListener([](SKSE::MessagingInterface::Message* message) {
        if (message->type == SKSE::MessagingInterface::kDataLoaded) {
            if (auto* input = RE::BSInputDeviceManager::GetSingleton()) {
                input->AddEventSink(InputSink::GetSingleton());
                SKSE::log::info("IngredientExporter ready - press F8 in-game to export ingredients");
            }
        }
    });

    return true;
}
