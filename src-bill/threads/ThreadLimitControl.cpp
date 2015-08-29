#include "ThreadLimitControl.h"
#include "../classes/AppBill.h"
#include "../classes/UdpControlClient.h"
#include "../common.h"

ThreadLimitControl::ThreadLimitControl() {
    id = idName();
    name = "Limit control";
}

bool ThreadLimitControl::ready() {
    if (!repository.billingData->ready()) {
        return false;
    }

    if (!repository.currentCalls->ready()) {
        return false;
    }


    return true;
}

void ThreadLimitControl::run() {


    if (!repository.prepare()) {
        return;
    }

    auto calls = repository.currentCalls->getCallsWaitingSaving();
    for (auto &call : *calls.get()) {

        if (limitControlKillNeeded(call)) {

            string phone = lexical_cast<string>(call.src_number);
            string call_id = lexical_cast<string>(call.cdr_call_id);
            UdpControlClient::kill(phone, call_id);

        }

    }

}

bool ThreadLimitControl::limitControlKillNeeded(Call &call) {

    if (call.account_id == 0) {
        return false;
    }

    auto client = repository.getAccount(call.account_id);
    if (client == nullptr) {
        Log::error("Account #" + lexical_cast<string>(call.account_id) + " not found");
        return false;
    }

    double vat_rate = repository.getVatRate(client);

    auto clientCounter = repository.billingData->clientCounter.get();
    ClientCounterObj c0 = clientCounter->get(call.account_id);
    ClientCounterObj c2 = repository.currentCalls->getClientCounter()->get(call.account_id);

    double globalBalanceSum, globalDaySum;
    fetchGlobalCounters(call.account_id, globalBalanceSum, globalDaySum, vat_rate);

    double spentBalanceSum, spentDaySum, spentMonthSum;
    spentBalanceSum = c0.sumBalance(vat_rate) + c2.sumBalance(vat_rate) + globalBalanceSum;
    spentDaySum = c0.sumDay(vat_rate) + c2.sumDay(vat_rate) + globalDaySum;

    if (call.trunk_service_id != 0) {

        if (client->isConsumedCreditLimit(spentBalanceSum)) {

            Log::notice(
                    "KILL: trunk #" + lexical_cast<string>(call.trunk_service_id) + ": Credit limit: " + string_fmt("%.2f", client->balance + client->credit + c0.sumBalance(vat_rate) + c2.sumBalance(vat_rate) + globalBalanceSum) + " = " +
                    string_fmt("%.2f", client->balance) + " (balance) + " + string_fmt("%d", client->credit) + " (credit) + " + string_fmt("%.2f", c0.sumBalance(vat_rate)) + " (local) + " + string_fmt("%.2f", c2.sumBalance(vat_rate)) + " (current) + " + string_fmt("%.2f", globalBalanceSum) + " (global) <br/>\n"
            );

            return true;

        }

    } else if (call.number_service_id != 0 && call.orig) {

        // Глобальная блокировка
        if (client->is_blocked) {
            Log::notice(
                    "KILL: number " + lexical_cast<string>(call.src_number) + " -> " + lexical_cast<string>(call.dst_number) + " : Block Global<br/>\n"
            );
            return true;
        }

        // Блокировка МГМН
        if (!call.isLocal() && client->disabled) {
            Log::notice(
                    "KILL: number " + lexical_cast<string>(call.src_number) + " -> " + lexical_cast<string>(call.dst_number) + " : Block MGMN<br/>\n"
            );
            return true;
        }

        // Глобальная блокировка если превышен лимит кредита и не оплачен последний счет
        if (client->isConsumedCreditLimit(spentBalanceSum) && client->last_payed_month < get_tmonth(time(nullptr))) {

            Log::notice(
                    "KILL: number " + lexical_cast<string>(call.src_number) + " -> " + lexical_cast<string>(call.dst_number) + " : Credit limit: " + string_fmt("%.2f", client->balance + client->credit + c0.sumBalance(vat_rate) + c2.sumBalance(vat_rate) + globalBalanceSum) + " = " +
                    string_fmt("%.2f", client->balance) + " (balance) + " + string_fmt("%d", client->credit) + " (credit) + " + string_fmt("%.2f", c0.sumBalance(vat_rate)) + " (local) + " + string_fmt("%.2f", c2.sumBalance(vat_rate)) + " (current) + " + string_fmt("%.2f", globalBalanceSum) + " (global) <br/>\n"
            );

            return true;
        }

        // Блокировка МГМН если превышен лимит кредита
        if (!call.isLocal()  && client->isConsumedCreditLimit(spentBalanceSum)) {

            Log::notice(
                    "KILL: number " + lexical_cast<string>(call.src_number) + " -> " + lexical_cast<string>(call.dst_number) + " : Credit limit: " + string_fmt("%.2f", client->balance + client->credit + c0.sumBalance(vat_rate) + c2.sumBalance(vat_rate) + globalBalanceSum) + " = " +
                    string_fmt("%.2f", client->balance) + " (balance) + " + string_fmt("%d", client->credit) + " (credit) + " + string_fmt("%.2f", c0.sumBalance(vat_rate)) + " (local) + " + string_fmt("%.2f", c2.sumBalance(vat_rate)) + " (current) + " + string_fmt("%.2f", globalBalanceSum) + " (global) <br/>\n"
            );

            return true;
        }

        // Блокировка МГМН если превышен дневной лимит
        if (!call.isLocal() && client->isConsumedDailyLimit(spentDaySum)) {

            Log::notice(
                    "KILL: number " + lexical_cast<string>(call.src_number) + " -> " + lexical_cast<string>(call.dst_number) + " : Daily limit: " + string_fmt("%.2f", client->limit_d + c0.sumDay(vat_rate) + c2.sumDay(vat_rate) + globalDaySum) + " = " +
                    string_fmt("%d", client->limit_d) + " (limit_d) + " + string_fmt("%.2f", c0.sumDay(vat_rate)) + " (local) + " + string_fmt("%.2f", c2.sumDay(vat_rate)) + " (current) + " + string_fmt("%.2f", globalDaySum) + " (global) <br/>\n"
            );

            return true;
        }

    }

    return false;

}

void ThreadLimitControl::fetchGlobalCounters(int accountId, double &globalBalanceSum, double &globalDaySum, double vat_rate) {

    GlobalCounters * globalCounter = nullptr;
    if (repository.data->globalCounters.ready()) {
        globalCounter = repository.data->globalCounters.get()->find(accountId);
    }

    if (globalCounter != nullptr) {
        globalBalanceSum = globalCounter->sumBalance(vat_rate);
        globalDaySum = globalCounter->sumDay(vat_rate);
    } else {
        globalBalanceSum = 0.0;
        globalDaySum = 0.0;
    }
}

