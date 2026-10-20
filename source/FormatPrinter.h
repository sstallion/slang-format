// Copyright 2026 Steven Stallion
// SPDX-License-Identifier: MIT

#pragma once

#include "Align.h"
#include "TokenHelper.h"
#include "slang/format/Style.h"

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

#include <slang/parsing/Token.h>
#include <slang/parsing/TokenKind.h>
#include <slang/syntax/AllSyntax.h>
#include <slang/syntax/SyntaxNode.h>
#include <slang/syntax/SyntaxTree.h>
#include <slang/syntax/SyntaxVisitor.h>

namespace slang::format {

/// Performs the single-pass tree walk that combines serialization, indentation, break handling, and
/// empty-line limiting. Structural changes are handled by the syntax rewriters before this pass.
class FormatPrinter : public slang::syntax::SyntaxVisitor<FormatPrinter> {
public:
    /// Formatted output together with the per-line metadata consumed by alignment.
    struct Result {
        std::string output;
        std::vector<LineMetadata> lineMetadata;
    };

    explicit FormatPrinter(const Style& style) : style(style) {}

    /// Formats \p tree and returns the output and line metadata.
    Result print(const slang::syntax::SyntaxTree& tree);

    /// Emits a token reached through the default visitor traversal.
    void visitToken(slang::parsing::Token tok);

    void handle(const slang::syntax::AnsiPortListSyntax& p);
    void handle(const slang::syntax::AssignmentPatternItemSyntax& item);
    void handle(const slang::syntax::AttributeInstanceSyntax& a);
    void handle(const slang::syntax::BinaryBinsSelectExprSyntax& expr);
    void handle(const slang::syntax::BinaryExpressionSyntax& expr);
    void handle(const slang::syntax::BinaryPropertyExprSyntax& expr);
    void handle(const slang::syntax::BinarySequenceExprSyntax& expr);
    void handle(const slang::syntax::BindDirectiveSyntax& bind);
    void handle(const slang::syntax::BinsSelectionSyntax& bs);
    void handle(const slang::syntax::BlockStatementSyntax& block);
    void handle(const slang::syntax::CaseGenerateSyntax& cg);
    void handle(const slang::syntax::CaseStatementSyntax& caseStmt);
    void handle(const slang::syntax::CheckerDeclarationSyntax& c);
    void handle(const slang::syntax::ClassDeclarationSyntax& c);
    void handle(const slang::syntax::ClockingDeclarationSyntax& c);
    void handle(const slang::syntax::CompilationUnitSyntax& cu);
    void handle(const slang::syntax::ConcurrentAssertionStatementSyntax& stmt);
    void handle(const slang::syntax::ConditionalExpressionSyntax& expr);
    void handle(const slang::syntax::ConditionalStatementSyntax& stmt);
    void handle(const slang::syntax::ConfigDeclarationSyntax& c);
    void handle(const slang::syntax::ConstraintBlockSyntax& b);
    void handle(const slang::syntax::ConstraintDeclarationSyntax& c);
    void handle(const slang::syntax::ContinuousAssignSyntax& assign);
    void handle(const slang::syntax::CoverCrossSyntax& c);
    void handle(const slang::syntax::CoverageBinsSyntax& cb);
    void handle(const slang::syntax::CovergroupDeclarationSyntax& c);
    void handle(const slang::syntax::CoverpointSyntax& c);
    void handle(const slang::syntax::DPIExportSyntax& decl);
    void handle(const slang::syntax::DPIImportSyntax& decl);
    void handle(const slang::syntax::DataDeclarationSyntax& decl);
    void handle(const slang::syntax::DefParamSyntax& decl);
    void handle(const slang::syntax::DefaultCaseItemSyntax& item);
    void handle(const slang::syntax::DefaultClockingReferenceSyntax& decl);
    void handle(const slang::syntax::DefaultDisableDeclarationSyntax& decl);
    void handle(const slang::syntax::DefaultRsCaseItemSyntax& item);
    void handle(const slang::syntax::DelayedSequenceElementSyntax& elem);
    void handle(const slang::syntax::DistConstraintListSyntax& dc);
    void handle(const slang::syntax::DistWeightSyntax& dw);
    void handle(const slang::syntax::DoWhileStatementSyntax& loop);
    void handle(const slang::syntax::ElabSystemTaskSyntax& task);
    void handle(const slang::syntax::EnumTypeSyntax& e);
    void handle(const slang::syntax::EqualsTypeClauseSyntax& clause);
    void handle(const slang::syntax::EqualsValueClauseSyntax& clause);
    void handle(const slang::syntax::ForLoopStatementSyntax& loop);
    void handle(const slang::syntax::ForeachLoopStatementSyntax& loop);
    void handle(const slang::syntax::ForeverStatementSyntax& loop);
    void handle(const slang::syntax::ForwardTypedefDeclarationSyntax& decl);
    void handle(const slang::syntax::FunctionDeclarationSyntax& f);
    void handle(const slang::syntax::FunctionPortListSyntax& pl);
    void handle(const slang::syntax::GenerateBlockSyntax& b);
    void handle(const slang::syntax::GenerateRegionSyntax& r);
    void handle(const slang::syntax::GenvarDeclarationSyntax& decl);
    void handle(const slang::syntax::HierarchicalInstanceSyntax& inst);
    void handle(const slang::syntax::HierarchyInstantiationSyntax& inst);
    void handle(const slang::syntax::IfGenerateSyntax& ig);
    void handle(const slang::syntax::ImmediateAssertionStatementSyntax& stmt);
    void handle(const slang::syntax::ImplicationConstraintSyntax& ic);
    void handle(const slang::syntax::InvocationExpressionSyntax& expr);
    void handle(const slang::syntax::LetDeclarationSyntax& decl);
    void handle(const slang::syntax::LoopGenerateSyntax& lg);
    void handle(const slang::syntax::LoopStatementSyntax& loop);
    void handle(const slang::syntax::ModportItemSyntax& m);
    void handle(const slang::syntax::ModuleDeclarationSyntax& module);
    void handle(const slang::syntax::MultipleConcatenationExpressionSyntax& expr);
    void handle(const slang::syntax::NamedBlockClauseSyntax& clause);
    void handle(const slang::syntax::NamedLabelSyntax& label);
    void handle(const slang::syntax::NamedParamAssignmentSyntax& param);
    void handle(const slang::syntax::NamedPortConnectionSyntax& conn);
    void handle(const slang::syntax::NetAliasSyntax& alias);
    void handle(const slang::syntax::NetDeclarationSyntax& decl);
    void handle(const slang::syntax::NetTypeDeclarationSyntax& decl);
    void handle(const slang::syntax::NewClassExpressionSyntax& expr);
    void handle(const slang::syntax::NonAnsiPortListSyntax& p);
    void handle(const slang::syntax::PackageExportAllDeclarationSyntax& decl);
    void handle(const slang::syntax::PackageExportDeclarationSyntax& decl);
    void handle(const slang::syntax::PackageImportDeclarationSyntax& decl);
    void handle(const slang::syntax::ParameterDeclarationStatementSyntax& decl);
    void handle(const slang::syntax::ParameterPortListSyntax& p);
    void handle(const slang::syntax::ParameterValueAssignmentSyntax& p);
    void handle(const slang::syntax::PathDeclarationSyntax& pd);
    void handle(const slang::syntax::PathDescriptionSyntax& pd);
    void handle(const slang::syntax::PortDeclarationSyntax& decl);
    void handle(const slang::syntax::ProceduralBlockSyntax& proc);
    void handle(const slang::syntax::ProductionSyntax& prod);
    void handle(const slang::syntax::PropertyDeclarationSyntax& decl);
    void handle(const slang::syntax::RandCaseItemSyntax& item);
    void handle(const slang::syntax::RandCaseStatementSyntax& stmt);
    void handle(const slang::syntax::RandSequenceStatementSyntax& stmt);
    void handle(const slang::syntax::RangeSelectSyntax& sel);
    void handle(const slang::syntax::RsCaseSyntax& c);
    void handle(const slang::syntax::SequenceDeclarationSyntax& decl);
    void handle(const slang::syntax::SequenceRepetitionSyntax& rep);
    void handle(const slang::syntax::SpecifyBlockSyntax& s);
    void handle(const slang::syntax::SpecparamDeclaratorSyntax& sp);
    void handle(const slang::syntax::StandardCaseItemSyntax& item);
    void handle(const slang::syntax::StandardRsCaseItemSyntax& item);
    void handle(const slang::syntax::StructUnionMemberSyntax& member);
    void handle(const slang::syntax::StructUnionTypeSyntax& s);
    void handle(const slang::syntax::TimeUnitsDeclarationSyntax& decl);
    void handle(const slang::syntax::TimingControlStatementSyntax& tcs);
    void handle(const slang::syntax::TypedefDeclarationSyntax& decl);
    void handle(const slang::syntax::UdpDeclarationSyntax& u);
    void handle(const slang::syntax::UdpEdgeFieldSyntax& ue);
    void handle(const slang::syntax::UdpEntrySyntax& ue);
    void handle(const slang::syntax::UdpInitialStmtSyntax& u);
    void handle(const slang::syntax::UserDefinedNetDeclarationSyntax& decl);
    void handle(const slang::syntax::ValueRangeExpressionSyntax& vr);
    void handle(const slang::syntax::VariableDimensionSyntax& dim);
    void handle(const slang::syntax::WildcardPortListSyntax& p);

private:
    const Style& style;
    std::string output;
    std::vector<LineMetadata> lineMetadata;
    LineMetadata currentLineMeta;
    size_t outputLineNumber = 0; ///< Current output line number (number of newlines written).
    size_t lineStart = 0;        ///< Index into output where the current line begins.
    unsigned depth = 0;
    unsigned lineDepth = 0;        ///< Depth when content was last emitted on the current line.
    unsigned parenAlignColumn = 0; ///< Absolute column for paren-aligned argument indentation.
    bool atLineStart = true;
    bool nextIsPrimary = true;
    bool portItemNextIndent = false;
    bool formatEnabled = true;
    unsigned emptyLineCount = 0;
    size_t triviaSkip = 0;
    bool beforeOperator = false;
    bool suppressSeparator = false;
    unsigned bracketDepth = 0;
    bool inPackedDimension = false;
    bool inUnpackedDimension = false;
    bool afterPackedDimCloseBracket = false;
    bool afterUnpackedDimCloseBracket = false;

    /// Groups the per-token flags that are set after each token emission and consumed before the
    /// next.
    struct AfterTokenFlags {
        bool comma = false;
        bool integerBase = false;
        bool openBrace = false;
        bool openBracket = false;
        bool openParen = false;
        bool operatorToken = false;
        bool semicolon = false;
    };

    AfterTokenFlags after;
    slang::parsing::TokenKind lastTokenKind = slang::parsing::TokenKind::Unknown;
    bool skipExpandedToken = false;
    bool skipSpacingCapture = false;
    bool spaceBeforeCloseBracePending = false;
    bool spaceBeforeCloseBracketPending = false;
    bool spaceBeforeCloseParenPending = false;
    bool expandNamedParams = false;
    std::optional<LineMetadata::Kind> nextLineKind;

    /// Returns true if the formatted output ends with a blank line.
    [[nodiscard]] bool outputHasBlankLine() const;

    /// Finalizes the current line's metadata and advances to a new line.
    void finalizeLine();

    /// Forces a statement break before \p tok when OneStatementPerLine is enabled.
    void forceStatementBreak(slang::parsing::Token tok);

    /// Returns true if a break should be inserted before a conditional body that is not a block
    /// statement.
    [[nodiscard]] bool shouldBreakConditionalBody(const slang::syntax::SyntaxNode& body) const;

    /// Returns true if a conditional body fits on the current line with compaction enabled.
    [[nodiscard]] bool shouldCompactConditionalBody(const slang::syntax::SyntaxNode& body) const;

    /// Returns true if adjacent time-units declarations should be compacted.
    [[nodiscard]] bool shouldCompactTimeUnits(const slang::syntax::MemberSyntax* prev,
                                              const slang::syntax::MemberSyntax* curr) const;

    /// Strips trailing spaces from output and emits a newline.
    void forceNewline();

    /// Inserts a blank line before \p firstToken unless one already exists.
    void insertBlankLineBefore(slang::parsing::Token firstToken);

    /// Returns true if a blank line should be suppressed.
    bool shouldSuppressBlankLine();

    /// Emits a single trivia element.
    void emitTrivia(const slang::parsing::Trivia& t);

    /// Emits raw text, tracking newlines.
    void emitRawText(std::string_view raw);

    /// Emits preprocessor directive trivia.
    void emitDirectiveTrivia(const slang::parsing::Trivia& t);

    /// Returns true if the formatter is inside a dimension context.
    [[nodiscard]] bool inDimension() const;

    /// Returns true if operators should be padded with spaces.
    [[nodiscard]] bool shouldSpaceAroundOperator() const;

    /// Returns true if trailing spaces should be stripped before \p kind.
    [[nodiscard]] static bool needsStripBefore(slang::parsing::TokenKind kind);

    /// Normalizes whitespace before an opening bracket.
    void normalizeBeforeOpen(slang::parsing::TokenKind kind);

    /// Normalizes whitespace before a closing bracket, brace, or paren.
    void normalizeBeforeClose(slang::parsing::TokenKind kind);

    /// Strips trailing spaces from the current line.
    void stripTrailingSpaces();

    /// Appends one space unless formatting is disabled, the output is at the start of a line, or it
    /// already ends with a space.
    void ensureSpace();

    /// Replaces any trailing spaces with exactly one space unless formatting is disabled or the
    /// output is at the start of a line.
    void ensureSingleSpace();

    /// Rewrites the whitespace before \p next to one space when \p wantSpace is true and to none
    /// otherwise, optionally suppressing the automatic separator for \p next.
    void normalizeSpaceBefore(slang::parsing::Token next, bool wantSpace,
                              bool suppressNextSeparator = false);

    /// Forces a line break before \p closeTok unless formatting is disabled, the output is already
    /// at the start of a line, or \p closeTok begins a new line in the source.
    void breakBeforeClose(slang::parsing::Token closeTok);

    /// Emits the range operator \p op with spacing determined by RangeSpacing, where \p rightFirst
    /// is the first token of the right operand.
    void emitRangeOperator(slang::parsing::Token op, slang::parsing::Token rightFirst); // NOLINT

    /// Emits standalone trivia of \p closeParen using port item indentation.
    void emitPortListEndTrivia(slang::parsing::Token closeParen);

    /// Emits \p closeParen as a port list boundary line.
    void emitPortListClose(slang::parsing::Token closeParen);

    /// Returns the indentation width for the current line.
    [[nodiscard]] size_t indentWidth() const;

    /// Emits indentation for the current line.
    void emitIndent();

    /// Emits deferred spaces from after-token flags.
    void emitDeferredSpaces();

    /// Pads output with spaces before a trailing comment.
    void padBeforeTrailingComment();

    /// Emits a separator between tokens when necessary.
    void emitSeparator(slang::parsing::Token tok);

    /// Captures source and output spacing for the current token.
    void captureTokenSpacing(slang::parsing::Token tok);

    /// Emits a token with its trivia, indentation, and spacing.
    void emitToken(slang::parsing::Token tok);

    /// Updates the after-token flags based on the just-emitted token.
    void updateAfterTokenFlags(slang::parsing::Token tok);

    /// Returns true if \p node is inside a conditional predicate.
    static bool isInsideControlCondition(const slang::syntax::SyntaxNode& node);

    /// Emits the leading trailing comment of \p tok, returning the number of trivia elements
    /// consumed.
    size_t emitLeadingTrailingComment(slang::parsing::Token tok);

    /// Emits standalone comment trivia of a closing token at the current body depth.
    void emitEndTokenTrivia(slang::parsing::Token tok);

    /// Emits bin-packed wrapped port list elements.
    void emitBinPackedWrapped(
        const slang::syntax::SeparatedSyntaxList<slang::syntax::FunctionPortBaseSyntax>& ports,
        size_t limit);

    /// Visits a separated list of port items.
    template<typename T>
    void visitSeparatedList(const slang::syntax::SeparatedSyntaxList<T>& list) {
        for (auto& elem : list.elems()) {
            if (elem.isNode()) {
                portItemNextIndent = true;
                nextIsPrimary = true;
                elem.node()->visit(*this);
                portItemNextIndent = false;
            }
            else if (elem.isToken()) {
                emitToken(elem.token());
            }
        }
    }

    /// Visits a separated list of port items with forced line breaks.
    template<typename T>
    void visitWrappedSeparatedList(const slang::syntax::SeparatedSyntaxList<T>& list) {
        for (auto& elem : list.elems()) {
            if (elem.isNode()) {
                if (!atLineStart && !hasLeadingNewline(elem.node()->getFirstToken())) {
                    forceNewline();
                }
                portItemNextIndent = true;
                nextIsPrimary = true;
                elem.node()->visit(*this);
                portItemNextIndent = false;
            }
            else if (elem.isToken()) {
                emitToken(elem.token());
            }
        }
    }

    /// Visits declarators, marking continuation lines.
    void visitDeclarators(
        const slang::syntax::SeparatedSyntaxList<slang::syntax::DeclaratorSyntax>& list);

    /// Returns the flat width of an argument list.
    [[nodiscard]] static size_t argumentListFlatWidth(
        const slang::syntax::SeparatedSyntaxList<slang::syntax::ArgumentSyntax>& params);

    /// Visits an argument list with optional wrapping.
    void visitArgumentList(const slang::syntax::ArgumentListSyntax& argList);

    /// Returns true if a line break should be inserted before \p arg.
    [[nodiscard]] bool shouldBreakBeforeArg(const slang::syntax::ArgumentSyntax& arg,
                                            bool isLast) const;

    /// Emits wrapped arguments.
    void emitWrappedArgs(
        const slang::syntax::SeparatedSyntaxList<slang::syntax::ArgumentSyntax>& params);

    /// Visits a statement body with appropriate indentation.
    void visitBody(const slang::syntax::StatementSyntax& body);

    /// Visits a procedural block body, unwrapping timing controls.
    void visitProceduralBody(const slang::syntax::StatementSyntax& stmt, BlockBreakStyle brkStyle);

    /// Returns true if a break should be inserted after a procedural keyword.
    static bool shouldBreakAfterProcedural(BlockBreakStyle brkStyle,
                                           const slang::syntax::StatementSyntax& body,
                                           bool oneStatementPerLine);

    /// Returns true if a break should be inserted before a procedural keyword.
    static bool shouldBreakBeforeProcedural(BlockBreakStyle brkStyle,
                                            const slang::syntax::StatementSyntax& stmt,
                                            bool oneStatementPerLine);

    /// Visits a case item clause with appropriate indentation.
    void visitCaseItemClause(const slang::syntax::SyntaxNode& clause);

    /// Visits members of a scoped construct.
    template<typename ListT>
    void visitMembers(const ListT& members, slang::parsing::Token endTok,
                      const slang::syntax::NamedBlockClauseSyntax* blockName) {
        depth++;
        for (auto* member : members) {
            nextIsPrimary = true;
            member->visit(*this);
        }

        nextIsPrimary = true;
        emitEndTokenTrivia(endTok);

        depth--;
        nextIsPrimary = true;
        emitToken(endTok);
        if (blockName != nullptr) {
            blockName->visit(*this);
        }
    }

    /// Returns true if all members fit on a single line within the column limit.
    template<typename MemberList>
    bool membersFitOnSingleLine(const MemberList& members) {
        size_t totalWidth = 0;
        for (auto* member : members) {
            const auto w = flatWidth(*member, true);
            if (w == 0) {
                return false;
            }
            totalWidth += (totalWidth > 0 ? 1 : 0) + w;
        }

        const auto currentCol = output.size() - lineStart;
        const auto needed = currentCol + 1 + totalWidth + 1;
        return style.ColumnLimit == 0 || needed <= style.ColumnLimit;
    }

    /// Emits members inline on a single line.
    template<typename MemberList>
    void emitMembersInline(const MemberList& members) {
        for (auto* member : members) {
            triviaSkip = member->getFirstToken().trivia().size();
            nextIsPrimary = true;
            member->visit(*this);
        }
    }
};

} // namespace slang::format
