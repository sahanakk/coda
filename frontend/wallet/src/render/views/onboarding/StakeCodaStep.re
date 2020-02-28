[@bs.scope "window"] [@bs.val] external openExternal: string => unit = "";

[@react.component]
let make = (~prevStep, ~closeOnboarding) => {
  <OnboardingTemplate
    heading="Stake Coda to Earn Rewards"
    description={
      <p>
        {React.string(
           "Once you receive funds from the faucet, head into Settings to delegate or stake your new funds.",
         )}
      </p>
    }
    miscLeft=
      <>
        <Spacer height=2.0 />
        <div className=OnboardingTemplate.Styles.buttonRow>
          <Button
            style=Button.HyperlinkBlue2
            label="Go Back"
            onClick={_ => prevStep()}
          />
          <Spacer width=1.5 />
          // <Button
          //   width=17.
          //   style=Button.HyperlinkBlue2
          //   label="Continue without Auto-Staking"
          //   onClick={_ => nextStep()}
          // />
          // <Spacer width=1.5 />
          <Button
            width=15.
            label="Take me to Coda Wallet"
            style=Button.HyperlinkBlue3
            onClick={_ => closeOnboarding()}
          />
        </div>
      </>
  />;
};
